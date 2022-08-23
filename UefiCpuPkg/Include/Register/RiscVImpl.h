/** @file
  RISC-V package definitions.

  Copyright (c) 2016 - 2022, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef RISCV_H_
#define RISCV_H_

#include <Uefi.h>
#include <Register/RiscV.h>

#define _ASM_FUNC(Name, Section)    \
  .global   Name                  ; \
  .section  #Section, "ax"        ; \
  .type     Name, %function       ; \
  .p2align  2                     ; \
  Name:

#define ASM_FUNC(Name)  _ASM_FUNC(ASM_PFX(Name), .text. ## Name)

typedef UINT64 RISC_V_REGS_PROTOTYPE;

//
// Structure for 128-bit value
//
typedef struct {
  UINT64    Value64_L;
  UINT64    Value64_H;
} RISCV_UINT128;

#endif
