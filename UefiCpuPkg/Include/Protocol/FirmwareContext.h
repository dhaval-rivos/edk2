/** @file
  SBI inline function calls.

  Copyright (c) 2021-2022, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FIRMWARE_CONTEXT_H_
#define _FIRMWARE_CONTEXT_H_

typedef struct {
  UINT64 BootHartId;
  VOID   *PeiServiceTable;    // PEI Service table
  UINT64 FlattenedDeviceTree; // Pointer to Flattened Device tree
} EFI_RISCV_FIRMWARE_CONTEXT;

#endif
