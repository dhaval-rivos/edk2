/** @file
  Memory Detection for Virtual Machines.

  Copyright (c) 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2006 - 2014, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:

  MemDetect.c

**/

//
// The package level header files this module uses
//
#include <PiPei.h>

//
// The Library classes this module consumes
//
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/ResourcePublicationLib.h>
#include <Library/RiscVEdk2SbiLib.h>

#include <libfdt.h>

#include <Guid/FdtHob.h>

#include "Platform.h"

STATIC EFI_PHYSICAL_ADDRESS  SystemMemoryBase;
STATIC UINT64                SystemMemorySize;
STATIC EFI_PHYSICAL_ADDRESS  MmodeResvBase;
STATIC UINT64                MmodeResvSize;

/**
  Publish PEI core memory.

  @return EFI_SUCCESS     The PEIM initialized successfully.

**/
EFI_STATUS
PublishPeiMemory (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_RISCV_FIRMWARE_CONTEXT  *FirmwareContext;
  VOID                                *FdtPointer;
  INT32         Node, Prev;
  CONST CHAR8   *Type;
  INT32         Len;
  CONST UINT64  *RegProp;
  UINT64        CurBase, CurSize;
  UINT64        NewBase = 0, NewSize = 0;
  EFI_PHYSICAL_ADDRESS  MemoryBase;
  UINT64                MemorySize;

  FirmwareContext = NULL;
  GetFirmwareContextPointer (&FirmwareContext);

  if (FirmwareContext == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: OpenSBI Firmware Context is NULL\n", __FUNCTION__));
    return EFI_UNSUPPORTED;
  }

  FdtPointer = (VOID *)FirmwareContext->FlattenedDeviceTree;
  if (FdtPointer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid FDT pointer\n", __FUNCTION__));
    return EFI_UNSUPPORTED;
  }

  // Look for the lowest memory node
  for (Prev = 0;; Prev = Node) {
    Node = fdt_next_node (FdtPointer, Prev, NULL);
    if (Node < 0) {
      break;
    }
    // Check for memory node
    Type = fdt_getprop (FdtPointer, Node, "device_type", &Len);
    if (Type && AsciiStrnCmp (Type, "memory", Len) == 0) {
      // Get the 'reg' property of this node. For now, we will assume
      // two 8 byte quantities for base and size, respectively.
      RegProp = fdt_getprop (FdtPointer, Node, "reg", &Len);
      if (RegProp != 0 && Len == (2 * sizeof (UINT64))) {

        CurBase = fdt64_to_cpu (ReadUnaligned64 (RegProp));
        CurSize = fdt64_to_cpu (ReadUnaligned64 (RegProp + 1));

        DEBUG ((DEBUG_INFO, "%a: System RAM @ 0x%lx - 0x%lx\n",
	__FUNCTION__, CurBase, CurBase + CurSize - 1));

        if (NewBase > CurBase || NewBase == 0) {
	NewBase = CurBase;
	NewSize = CurSize;
        }
      } else {
        DEBUG ((DEBUG_ERROR, "%a: Failed to parse FDT memory node\n",
	__FUNCTION__));
      }
    }
  }

  SystemMemoryBase = NewBase;
  SystemMemorySize = NewSize;

  /* try to locate the reserved memory opensbi node */
  Node = fdt_path_offset(FdtPointer, "/reserved-memory/mmode_resv0");
  if (Node >= 0) {
    RegProp = fdt_getprop (FdtPointer, Node, "reg", &Len);
    if (RegProp != 0 && Len == (2 * sizeof (UINT64))) {
      NewBase = fdt64_to_cpu (ReadUnaligned64 (RegProp));
      NewSize = fdt64_to_cpu (ReadUnaligned64 (RegProp + 1));
      DEBUG ((DEBUG_INFO, "%a: M-mode Base = 0x%lx, M-mode Size = 0x%lx\n",
	    __FUNCTION__, NewBase, NewSize));
      MmodeResvBase =  NewBase;
      MmodeResvSize = NewSize;
    }
  }

  DEBUG ((DEBUG_INFO, "%a: SystemMemoryBase:0x%x SystemMemorySize:%x\n",
          __FUNCTION__, SystemMemoryBase, SystemMemorySize));

  MemoryBase = SystemMemoryBase + 0x1000000UL; // Initial 16MB needs to be reserved
  if ((MmodeResvBase + MmodeResvSize) < MemoryBase) {
    MemorySize = SystemMemorySize - 0x1000000UL;
  }
  else {
    MemorySize = MmodeResvBase - SystemMemoryBase;
  }

  //
  // Publish this memory to the PEI Core
  //
  Status = PublishSystemMemory (MemoryBase, MemorySize);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Publish system RAM and reserve memory regions.

**/
VOID
InitializeRamRegions (
  VOID
  )
{
  UINT64 Start, End;

  Start = SystemMemoryBase + 0x1000000UL;
  End = MmodeResvBase;
  if (End >= Start) {
    AddMemoryRangeHob(Start, End);
    DEBUG ((DEBUG_INFO, "%a: PEI Memory Start:0x%lx End:0x%lx\n",
            __FUNCTION__, Start, End));

    AddReservedMemoryBaseSizeHob(MmodeResvBase, MmodeResvSize);

    Start = MmodeResvBase + MmodeResvSize;
    End = SystemMemoryBase + SystemMemorySize;
    DEBUG ((DEBUG_INFO, "%a: M-Mode FW Start:0x%lx End:0x%lx\n",
            __FUNCTION__, Start, End));
    AddMemoryRangeHob(Start, End);

    BuildMemoryAllocationHob (SystemMemoryBase,
                              SystemMemorySize,
                              EfiReservedMemoryType
                             );
  }
  else {
    End = Start + SystemMemorySize - 0x1000000UL;
    AddMemoryRangeHob(Start, End);
  }
}
