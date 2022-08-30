/** @file
  RISC-V CPU library definitions.

  Copyright (c) 2016 - 2022, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef RISCV_CPU_LIB_H_
#define RISCV_CPU_LIB_H_

VOID
RiscVSetSupervisorScratch (
  UINT64
  );

UINT64
RiscVGetSupervisorScratch (
  VOID
  );

#endif
