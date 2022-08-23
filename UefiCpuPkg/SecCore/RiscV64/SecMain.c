/** @file
  RISC-V SEC phase module.

  Copyright (c) 2021-2022, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SecMain.h"

EFI_STATUS
EFIAPI
TemporaryRamMigration (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PHYSICAL_ADDRESS     TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS     PermanentMemoryBase,
  IN UINTN                    CopySize
  );

EFI_STATUS
EFIAPI
TemporaryRamDone (
  VOID
  );

STATIC EFI_PEI_TEMPORARY_RAM_SUPPORT_PPI mTemporaryRamSupportPpi = {
  TemporaryRamMigration
};

STATIC EFI_PEI_TEMPORARY_RAM_DONE_PPI mTemporaryRamDonePpi = {
  TemporaryRamDone
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPrivateDispatchTable[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiTemporaryRamSupportPpiGuid,
    &mTemporaryRamSupportPpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiTemporaryRamDonePpiGuid,
    &mTemporaryRamDonePpi
  },
};

/** Temporary RAM migration function.

  This function migrates the data from temporary RAM to permanent
  memory.

  @param[in]  PeiServices           PEI service
  @param[in]  TemporaryMemoryBase   Temporary memory base address
  @param[in]  PermanentMemoryBase   Permanent memory base address
  @param[in]  CopySize              Size to copy

**/
EFI_STATUS
EFIAPI
TemporaryRamMigration (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PHYSICAL_ADDRESS     TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS     PermanentMemoryBase,
  IN UINTN                    CopySize
  )
{
  VOID      *OldHeap;
  VOID      *NewHeap;
  VOID      *OldStack;
  VOID      *NewStack;
  EFI_RISCV_FIRMWARE_CONTEXT *FirmwareContext;

  DEBUG ((DEBUG_INFO,
    "%a: Temp Mem Base:0x%Lx, Permanent Mem Base:0x%Lx, CopySize:0x%Lx\n",
    __FUNCTION__,
    TemporaryMemoryBase,
    PermanentMemoryBase,
    (UINT64)CopySize
    ));

  OldHeap = (VOID*)(UINTN)TemporaryMemoryBase;
  NewHeap = (VOID*)((UINTN)PermanentMemoryBase + (CopySize >> 1));

  OldStack = (VOID*)((UINTN)TemporaryMemoryBase + (CopySize >> 1));
  NewStack = (VOID*)(UINTN)PermanentMemoryBase;

  CopyMem (NewHeap, OldHeap, CopySize >> 1);   // Migrate Heap
  CopyMem (NewStack, OldStack, CopySize >> 1); // Migrate Stack

  //
  // Reset firmware context pointer
  //
  GetFirmwareContextPointer (&FirmwareContext);
  FirmwareContext = (VOID *)FirmwareContext + (unsigned long)((UINTN)NewStack - (UINTN)OldStack);
  SetFirmwareContextPointer (FirmwareContext);

  DEBUG ((DEBUG_INFO, "%a: Firmware Context is relocated to 0x%x\n", __FUNCTION__, FirmwareContext));

  register UINTN  a0 asm ("a0") = (UINTN)((UINTN)NewStack - (UINTN)OldStack);
  asm volatile ("add sp, sp, a0"::"r"(a0):);
  return EFI_SUCCESS;
}

/** Temprary RAM done function.

**/
EFI_STATUS EFIAPI TemporaryRamDone (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "%a: 2nd time PEI core, temporary ram done.\n", __FUNCTION__));
  return EFI_SUCCESS;
}
/** Return platform SEC PPI before PEI Core

  @param[in,out]  ThisPpiList   Pointer to retrieve EFI_PEI_PPI_DESCRIPTOR.

**/
STATIC EFI_STATUS
GetPlatformPrePeiCorePpiDescriptor (
  IN OUT EFI_PEI_PPI_DESCRIPTOR  **ThisPpiList
)
{
  *ThisPpiList = mPrivateDispatchTable;
  return EFI_SUCCESS;
}

/**

  Entry point to the C language phase of SEC. After the SEC assembly
  code has initialized some temporary memory and set up the stack,
  the control is transferred to this function.


  @param[in]  BootHartId         Hardware thread ID of boot hart.
  @param[in]  DeviceTreeAddress  Pointer to Device Tree (DTB)
**/
VOID
NORETURN
EFIAPI
SecStartup (
  IN  UINTN BootHartId,
  IN  VOID *DeviceTreeAddress
  )
{
  EFI_RISCV_FIRMWARE_CONTEXT FirmwareContext;
  EFI_FIRMWARE_VOLUME_HEADER *BootFv;
  EFI_FIRMWARE_VOLUME_HEADER *SecFv;
  EFI_PEI_CORE_ENTRY_POINT   PeiCoreEntryPoint;
  EFI_PEI_PPI_DESCRIPTOR     *PpiList;
  EFI_SEC_PEI_HAND_OFF       SecCoreData;
  EFI_STATUS                 Status;

  //
  // Report Status Code to indicate entering SEC core
  //
#if 0
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_SEC | EFI_SW_SEC_PC_ENTRY_POINT
    );
#endif
  DEBUG ((
    DEBUG_INFO,
    "%a() BootHartId: 0x%x, DeviceTreeAddress=0x%x\n",
    __FUNCTION__,
    BootHartId,
    DeviceTreeAddress
    ));

  //
  // Process all libraries constructor function linked to SecCore.
  //
  ProcessLibraryConstructorList ();

  BootFv = (EFI_FIRMWARE_VOLUME_HEADER *)FixedPcdGet32 (PcdRiscVPeiFvBase);
  SecFv = (EFI_FIRMWARE_VOLUME_HEADER *)FixedPcdGet32 (PcdRiscVSecFvBase);

  ASSERT(BootFv != NULL);
  SecCoreData.DataSize               = (UINT16)sizeof (EFI_SEC_PEI_HAND_OFF);
  SecCoreData.BootFirmwareVolumeBase = BootFv;
  SecCoreData.BootFirmwareVolumeSize = (UINTN)BootFv->FvLength;
  SecCoreData.TemporaryRamBase       = (VOID *)(UINT64)FixedPcdGet32 (PcdTemporaryRamBase);
  SecCoreData.TemporaryRamSize       = (UINTN)FixedPcdGet32 (PcdTemporaryRamSize);
  SecCoreData.PeiTemporaryRamBase    = SecCoreData.TemporaryRamBase;
  SecCoreData.PeiTemporaryRamSize    = SecCoreData.TemporaryRamSize >> 1;
  SecCoreData.StackBase              = (UINT8 *)SecCoreData.TemporaryRamBase + (SecCoreData.TemporaryRamSize >> 1);
  SecCoreData.StackSize              = SecCoreData.TemporaryRamSize >> 1;

  DEBUG ((
    DEBUG_INFO,
    "%a() BFV Base: 0x%x, BFV Size: 0x%x, TempRAM Base: 0x%x, TempRAM Size: 0x%x, PeiTempRamBase: 0x%x, PeiTempRamSize: 0x%x, StackBase: 0x%x, StackSize: 0x%x\n",
    __FUNCTION__,
    SecCoreData.BootFirmwareVolumeBase,
    SecCoreData.BootFirmwareVolumeSize,
    SecCoreData.TemporaryRamBase,
    SecCoreData.TemporaryRamSize,
    SecCoreData.PeiTemporaryRamBase,
    SecCoreData.PeiTemporaryRamSize,
    SecCoreData.StackBase,
    SecCoreData.StackSize
    ));

  //
  // Both SecCore and PeiCore are in BFV.
  //
  FindAndReportEntryPoints (
    (EFI_FIRMWARE_VOLUME_HEADER *)SecFv,
    (EFI_FIRMWARE_VOLUME_HEADER *)BootFv,
    &PeiCoreEntryPoint
    );
  if (PeiCoreEntryPoint == NULL) {
    CpuDeadLoop ();
  }

  Status = GetPlatformPrePeiCorePpiDescriptor (&PpiList);
  if (EFI_ERROR (Status)) {
    PpiList = NULL;
  }

  FirmwareContext.BootHartId = BootHartId;
  FirmwareContext.FlattenedDeviceTree = (UINT64)DeviceTreeAddress;
  SetFirmwareContextPointer (&FirmwareContext);

  //
  // Report Status Code to indicate transferring to PEI core
  //
#if 0
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_SEC | EFI_SW_SEC_PC_HANDOFF_TO_NEXT
    );
#endif
  //
  // Transfer the control to the PEI core
  //
  ASSERT (PeiCoreEntryPoint != NULL);
  (*PeiCoreEntryPoint)(&SecCoreData, PpiList);

  //
  // Should not come here.
  //
  UNREACHABLE ();
}
