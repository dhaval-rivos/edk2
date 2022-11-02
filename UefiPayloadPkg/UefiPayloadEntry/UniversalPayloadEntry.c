/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UefiPayloadEntry.h"
#include <Library/BaseRiscVSbiLib.h>
#include <libfdt.h>

#define MEMORY_ATTRIBUTE_MASK  (EFI_RESOURCE_ATTRIBUTE_PRESENT             |        \
                                       EFI_RESOURCE_ATTRIBUTE_INITIALIZED         | \
                                       EFI_RESOURCE_ATTRIBUTE_TESTED              | \
                                       EFI_RESOURCE_ATTRIBUTE_READ_PROTECTED      | \
                                       EFI_RESOURCE_ATTRIBUTE_WRITE_PROTECTED     | \
                                       EFI_RESOURCE_ATTRIBUTE_EXECUTION_PROTECTED | \
                                       EFI_RESOURCE_ATTRIBUTE_READ_ONLY_PROTECTED | \
                                       EFI_RESOURCE_ATTRIBUTE_16_BIT_IO           | \
                                       EFI_RESOURCE_ATTRIBUTE_32_BIT_IO           | \
                                       EFI_RESOURCE_ATTRIBUTE_64_BIT_IO           | \
                                       EFI_RESOURCE_ATTRIBUTE_PERSISTENT          )

#define TESTED_MEMORY_ATTRIBUTES  (EFI_RESOURCE_ATTRIBUTE_PRESENT     |     \
                                       EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
                                       EFI_RESOURCE_ATTRIBUTE_TESTED      )

extern VOID  *mHobList;

/**
  Print all HOBs info from the HOB list.

  @return The pointer to the HOB list.
**/
VOID
PrintHob (
  IN CONST VOID  *HobStart
  );

/**
  Some bootloader may pass a pcd database, and UPL also contain a PCD database.
  Dxe PCD driver has the assumption that the two PCD database can be catenated and
  the local token number should be successive.
  This function will fix up the UPL PCD database to meet that assumption.

  @param[in]   DxeFv         The FV where to find the Universal PCD database.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval other              Failed to fix up.
**/
EFI_STATUS
FixUpPcdDatabase (
  IN  EFI_FIRMWARE_VOLUME_HEADER  *DxeFv
  )
{
  EFI_STATUS           Status;
  EFI_FFS_FILE_HEADER  *FileHeader;
  VOID                 *PcdRawData;
  PEI_PCD_DATABASE     *PeiDatabase;
  PEI_PCD_DATABASE     *UplDatabase;
  EFI_HOB_GUID_TYPE    *GuidHob;
  DYNAMICEX_MAPPING    *ExMapTable;
  UINTN                Index;

  GuidHob = GetFirstGuidHob (&gPcdDataBaseHobGuid);
  if (GuidHob == NULL) {
    //
    // No fix-up is needed.
    //
    return EFI_SUCCESS;
  }

  PeiDatabase = (PEI_PCD_DATABASE *)GET_GUID_HOB_DATA (GuidHob);
  DEBUG ((DEBUG_INFO, "Find the Pei PCD data base, the total local token number is %d\n", PeiDatabase->LocalTokenCount));

  Status = FvFindFileByTypeGuid (DxeFv, EFI_FV_FILETYPE_DRIVER, PcdGetPtr (PcdPcdDriverFile), &FileHeader);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FileFindSection (FileHeader, EFI_SECTION_RAW, &PcdRawData);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UplDatabase = (PEI_PCD_DATABASE *)PcdRawData;
  ExMapTable  = (DYNAMICEX_MAPPING *)(UINTN)((UINTN)PcdRawData + UplDatabase->ExMapTableOffset);

  for (Index = 0; Index < UplDatabase->ExTokenCount; Index++) {
    ExMapTable[Index].TokenNumber += PeiDatabase->LocalTokenCount;
  }

  DEBUG ((DEBUG_INFO, "Fix up UPL PCD database successfully\n"));
  return EFI_SUCCESS;
}

/**
  Add HOB into HOB list

  @param[in]  Hob    The HOB to be added into the HOB list.
**/
VOID
AddNewHob (
  IN EFI_PEI_HOB_POINTERS  *Hob
  )
{
  EFI_PEI_HOB_POINTERS  NewHob;

  if (Hob->Raw == NULL) {
    return;
  }

  NewHob.Header = CreateHob (Hob->Header->HobType, Hob->Header->HobLength);

  if (NewHob.Header != NULL) {
    CopyMem (NewHob.Header + 1, Hob->Header + 1, Hob->Header->HobLength - sizeof (EFI_HOB_GENERIC_HEADER));
  }
}

/**
  Found the Resource Descriptor HOB that contains a range (Base, Top)

  @param[in] HobList    Hob start address
  @param[in] Base       Memory start address
  @param[in] Top        Memory end address.

  @retval     The pointer to the Resource Descriptor HOB.
**/
EFI_HOB_RESOURCE_DESCRIPTOR *
FindResourceDescriptorByRange (
  IN VOID                  *HobList,
  IN EFI_PHYSICAL_ADDRESS  Base,
  IN EFI_PHYSICAL_ADDRESS  Top
  )
{
  EFI_PEI_HOB_POINTERS         Hob;
  EFI_HOB_RESOURCE_DESCRIPTOR  *ResourceHob;

  for (Hob.Raw = (UINT8 *)HobList; !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
    //
    // Skip all HOBs except Resource Descriptor HOBs
    //
    if (GET_HOB_TYPE (Hob) != EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      continue;
    }

    //
    // Skip Resource Descriptor HOBs that do not describe tested system memory
    //
    ResourceHob = Hob.ResourceDescriptor;
    if (ResourceHob->ResourceType != EFI_RESOURCE_SYSTEM_MEMORY) {
      continue;
    }

    if ((ResourceHob->ResourceAttribute & MEMORY_ATTRIBUTE_MASK) != TESTED_MEMORY_ATTRIBUTES) {
      continue;
    }

    //
    // Skip Resource Descriptor HOBs that do not contain the PHIT range EfiFreeMemoryBottom..EfiFreeMemoryTop
    //
    if (Base < ResourceHob->PhysicalStart) {
      continue;
    }

    if (Top > (ResourceHob->PhysicalStart + ResourceHob->ResourceLength)) {
      continue;
    }

    return ResourceHob;
  }

  return NULL;
}

/**
  Find the highest below 4G memory resource descriptor, except the input Resource Descriptor.

  @param[in] HobList                 Hob start address
  @param[in] MinimalNeededSize       Minimal needed size.
  @param[in] ExceptResourceHob       Ignore this Resource Descriptor.

  @retval     The pointer to the Resource Descriptor HOB.
**/
EFI_HOB_RESOURCE_DESCRIPTOR *
FindAnotherHighestBelow4GResourceDescriptor (
  IN VOID                         *HobList,
  IN UINTN                        MinimalNeededSize,
  IN EFI_HOB_RESOURCE_DESCRIPTOR  *ExceptResourceHob
  )
{
  EFI_PEI_HOB_POINTERS         Hob;
  EFI_HOB_RESOURCE_DESCRIPTOR  *ResourceHob;
  EFI_HOB_RESOURCE_DESCRIPTOR  *ReturnResourceHob;

  ReturnResourceHob = NULL;

  for (Hob.Raw = (UINT8 *)HobList; !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
    //
    // Skip all HOBs except Resource Descriptor HOBs
    //
    if (GET_HOB_TYPE (Hob) != EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      continue;
    }

    //
    // Skip Resource Descriptor HOBs that do not describe tested system memory
    //
    ResourceHob = Hob.ResourceDescriptor;
    if (ResourceHob->ResourceType != EFI_RESOURCE_SYSTEM_MEMORY) {
      continue;
    }

    if ((ResourceHob->ResourceAttribute & MEMORY_ATTRIBUTE_MASK) != TESTED_MEMORY_ATTRIBUTES) {
      continue;
    }

    //
    // Skip if the Resource Descriptor HOB equals to ExceptResourceHob
    //
    if (ResourceHob == ExceptResourceHob) {
      continue;
    }

    //
    // Skip Resource Descriptor HOBs that are beyond 4G
    //
    if ((ResourceHob->PhysicalStart + ResourceHob->ResourceLength) > BASE_4GB) {
      continue;
    }

    //
    // Skip Resource Descriptor HOBs that are too small
    //
    if (ResourceHob->ResourceLength < MinimalNeededSize) {
      continue;
    }

    //
    // Return the topest Resource Descriptor
    //
    if (ReturnResourceHob == NULL) {
      ReturnResourceHob = ResourceHob;
    } else {
      if (ReturnResourceHob->PhysicalStart < ResourceHob->PhysicalStart) {
        ReturnResourceHob = ResourceHob;
      }
    }
  }

  return ReturnResourceHob;
}

/**
  Check the HOB and decide if it is need inside Payload

  Payload maintainer may make decision which HOB is need or needn't
  Then add the check logic in the function.

  @param[in] Hob The HOB to check

  @retval TRUE  If HOB is need inside Payload
  @retval FALSE If HOB is needn't inside Payload
**/
BOOLEAN
IsHobNeed (
  EFI_PEI_HOB_POINTERS  Hob
  )
{
  if (Hob.Header->HobType == EFI_HOB_TYPE_HANDOFF) {
    return FALSE;
  }

  if (Hob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION) {
    if (CompareGuid (&Hob.MemoryAllocationModule->MemoryAllocationHeader.Name, &gEfiHobMemoryAllocModuleGuid)) {
      return FALSE;
    }
  }

  // Arrive here mean the HOB is need
  return TRUE;
}

/**
  It will build HOBs based on information from bootloaders.

  @param[in]  fdt   The starting memory address of bootloader parameter block.
  @param[out] DxeFv                 The pointer to the DXE FV in memory.

  @retval EFI_SUCCESS        If it completed successfully.
  @retval Others             If it failed to build required HOBs.
**/
EFI_STATUS
BuildHobs (
  IN  const void*                 fdt,
  OUT EFI_FIRMWARE_VOLUME_HEADER  **DxeFv
  )
{
  UINTN                         MinimalNeededSize;
  EFI_PHYSICAL_ADDRESS          FreeMemoryBottom;
  EFI_PHYSICAL_ADDRESS          FreeMemoryTop;
  EFI_PHYSICAL_ADDRESS          MemoryBottom;
  UINT8                         *GuidHob;
  UNIVERSAL_PAYLOAD_ACPI_TABLE  *AcpiTable;
  ACPI_BOARD_INFO               *AcpiBoardInfo;
  EFI_RESOURCE_ATTRIBUTE_TYPE   Attribue;
  MEMORY_MAP_ENTRY              MemoryMap;
  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *UniversalSerialPort;
  VOID                                *NewBase;
  UINTN                               FdtSize;
  UINTN                               FdtPages;
  UINT64                              *FdtHobData;

  MinimalNeededSize = FixedPcdGet32 (PcdSystemMemoryUefiRegionSize);

  ASSERT ((UINT8 *)fdt != NULL);

    // HOB region is used for HOB and memory allocation for this module
  MemoryBottom    = PcdGet32 (PcdPayloadFdMemBase);
  FreeMemoryBottom = ALIGN_VALUE (MemoryBottom + PcdGet32 (PcdPayloadFdMemSize), SIZE_1MB);
  FreeMemoryTop  = FreeMemoryBottom + FixedPcdGet32 (PcdSystemMemoryUefiRegionSize);

  HobConstructor ((VOID *)(UINTN)MemoryBottom, (VOID *)(UINTN)FreeMemoryTop, (VOID *)(UINTN)FreeMemoryBottom, (VOID *)(UINTN)FreeMemoryTop);
  //
  // Build serial port info
  //
  UniversalSerialPort = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));
  ASSERT (UniversalSerialPort != NULL);
  UniversalSerialPort->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
  UniversalSerialPort->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);
  UniversalSerialPort->UseMmio         = TRUE;
  UniversalSerialPort->RegisterBase    = 0x10000000;
  UniversalSerialPort->BaudRate        = 115200;
  UniversalSerialPort->RegisterStride  = 1;

  // Call constructor for all libraries
  ProcessLibraryConstructorList ();

  DEBUG ((DEBUG_INFO, "Entering Universal Payload...\n"));
  DEBUG ((DEBUG_INFO, "sizeof(UINTN) = 0x%x\n", sizeof (UINTN)));

  MemoryMap.Base = 0x80000000;
  MemoryMap.Size = 0x40000000;
  MemoryMap.Type = EFI_RESOURCE_SYSTEM_MEMORY;
  MemoryMap.Flag = 0;

  Attribue = EFI_RESOURCE_ATTRIBUTE_PRESENT |
             EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
             EFI_RESOURCE_ATTRIBUTE_TESTED |
             EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;

  BuildResourceDescriptorHob (MemoryMap.Type, Attribue, (EFI_PHYSICAL_ADDRESS)MemoryMap.Base, MemoryMap.Size);

  // Build the CPU HOB with guest RAM size dependent address width and 16-bits
  // of IO space. (Side note: unlike other HOBs, the CPU HOB is needed during
  // S3 resume as well, so we build it unconditionally.)
  //
  // TODO: Determine this dynamically from the platform
  // setting or the HART configuration.
  //
  BuildCpuHob (48, 32);

  ASSERT ((UINT8 *)fdt != NULL);
  ASSERT (fdt_check_header (fdt) == 0);

  FdtSize  = fdt_totalsize (fdt);
  FdtPages = EFI_SIZE_TO_PAGES (FdtSize);
  NewBase  = AllocatePages (FdtPages);
  ASSERT (NewBase != NULL);
  fdt_open_into (fdt, NewBase, EFI_PAGES_TO_SIZE (FdtPages));

  FdtHobData = BuildGuidHob (&gFdtHobGuid, sizeof *FdtHobData);
  ASSERT (FdtHobData != NULL);
  *FdtHobData = (UINTN)NewBase;

  //
  // Create guid hob for acpi board information
  //
  GuidHob = GetFirstGuidHob (&gUniversalPayloadAcpiTableGuid);
  if (GuidHob != NULL) {
    AcpiTable     = (UNIVERSAL_PAYLOAD_ACPI_TABLE *)GET_GUID_HOB_DATA (GuidHob);
    AcpiBoardInfo = BuildHobFromAcpi ((UINT64)AcpiTable->Rsdp);
    ASSERT (AcpiBoardInfo != NULL);
  }

  *DxeFv = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)PcdGet32 (PcdPayloadFdMemBase);
  return EFI_SUCCESS;
}

/**
  Entry point to the C language phase of UEFI payload.
  Bootloader in this case contains FDT entry

  @param[in]   fdt    The starting address of bootloader parameter block.

  @retval      It will not return if SUCCESS, and return error when passing bootloader parameter.
**/
EFI_STATUS
EFIAPI
_ModuleEntryPoint (
  IN UINTN  BootHartId,
  IN UINTN  fdt
  )
{
  EFI_STATUS                  Status;
  PHYSICAL_ADDRESS            DxeCoreEntryPoint;
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_FIRMWARE_VOLUME_HEADER  *DxeFv;
  EFI_RISCV_FIRMWARE_CONTEXT  FirmwareContext;

  DxeFv    = NULL;
  FirmwareContext.BootHartId          = BootHartId;
  FirmwareContext.FlattenedDeviceTree = (UINT64)fdt;
  SetFirmwareContextPointer (&FirmwareContext);

  // Initialize floating point operating environment to be compliant
  // with UEFI spec.
  //InitializeFloatingPointUnits (); //TODO: Remove this Arch specific.

  // Build HOB based on information from Bootloader
  Status = BuildHobs ((const void*)fdt, &DxeFv);
  ASSERT_EFI_ERROR (Status);

  DEBUG_CODE (
    //
    // Dump the Hobs from boot loader
    //
    PrintHob (mHobList);
    );

  FixUpPcdDatabase (DxeFv);
  Status = UniversalLoadDxeCore (DxeFv, &DxeCoreEntryPoint);
  ASSERT_EFI_ERROR (Status);

 // DEBUG_CODE (
    //
    // Dump the Hobs from boot loader
    //
//    PrintHob (mHobList);
//    );

  //
  // Mask off all legacy 8259 interrupt sources
  //
  //IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER, 0xFF); //TODO: Remove this Arch specific.
  //IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE, 0xFF); //TODO: Remove this Arch specific.

  Hob.HandoffInformationTable = (EFI_HOB_HANDOFF_INFO_TABLE *)GetFirstHob (EFI_HOB_TYPE_HANDOFF);
  HandOffToDxeCore (DxeCoreEntryPoint, Hob);

  // Should not get here
  CpuDeadLoop ();
  return EFI_SUCCESS;
}
