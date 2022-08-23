//------------------------------------------------------------------------------
//
// RISC-V CPU functions.
//
// Copyright (c) 2016 - 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
// Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>
//
// SPDX-License-Identifier: BSD-2-Clause-Patent
//
//------------------------------------------------------------------------------
#include <Base.h>
#include <Register/RiscVImpl.h>

.data

.text
.align 3

//
// Set Supervisor mode scratch.
// @param a0 : Value set to Supervisor mode scratch
//
ASM_FUNC (RiscVSetSupervisorScratch)
    csrrw a1, RISCV_CSR_SUPERVISOR_SSCRATCH, a0
    ret

//
// Get Supervisor mode scratch.
// @retval a0 : Value in Supervisor mode scratch
//
ASM_FUNC (RiscVGetSupervisorScratch)
    csrr a0, RISCV_CSR_SUPERVISOR_SSCRATCH
    ret

//
// Set Supervisor mode trap vector.
// @param a0 : Value set to Supervisor mode trap vector
//
ASM_FUNC (RiscVSetSupervisorStvec)
    csrrw a1, RISCV_CSR_SUPERVISOR_STVEC, a0
    ret

//
// Get Supervisor mode trap vector.
// @retval a0 : Value in Supervisor mode trap vector
//
ASM_FUNC (RiscVGetSupervisorStvec)
    csrr a0, RISCV_CSR_SUPERVISOR_STVEC
    ret

//
// Set Supervisor Address Translation and
// Protection Register.
//
ASM_FUNC (RiscVSetSupervisorAddressTranslationRegister)
    csrw  RISCV_CSR_SUPERVISOR_SATP, a0
    ret

