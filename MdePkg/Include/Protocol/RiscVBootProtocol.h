/** @file
  RISC-V Boot Protocol mandatory for RISC-V UEFI platforms.

  Specification available at
   https://github.com/riscv-non-isa/riscv-uefi/releases/download/1.0.0/RISCV_UEFI_PROTOCOL-spec.pdf

  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _RISCV_BOOT_PROTOCOL_H_
#define _RISCV_BOOT_PROTOCOL_H_

// extern EFI_GUID  gRiscVEfiBootProtocolGuid;

typedef struct _RISCV_EFI_BOOT_PROTOCOL RISCV_EFI_BOOT_PROTOCOL;

#define RISCV_EFI_BOOT_PROTOCOL_REVISION  0x00010000
#define RISCV_EFI_BOOT_PROTOCOL_LATEST_VERSION \
        RISCV_EFI_BOOT_PROTOCOL_REVISION

typedef EFI_STATUS
(EFIAPI *EFI_GET_BOOT_HARTID)(
  IN RISCV_EFI_BOOT_PROTOCOL   *This,
  OUT UINTN                    *BootHartId
  );

typedef struct _RISCV_EFI_BOOT_PROTOCOL {
  UINT64                 Revision;
  EFI_GET_BOOT_HARTID    GetBootHartId;
} RISCV_EFI_BOOT_PROTOCOL;

#endif
