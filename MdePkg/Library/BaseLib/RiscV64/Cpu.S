//------------------------------------------------------------------------------
//
// Generic CPU related functions for RISC-V
//
// Copyright (c) 2020, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
// Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>
//
// SPDX-License-Identifier: BSD-2-Clause-Patent
//
//------------------------------------------------------------------------------

#include <Register/RiscV64/RiscVImpl.h>

.data
.align 3
.section .text

//
// Set Supervisor mode scratch.
// @param a0 : Value set to Supervisor mode scratch
//
ASM_FUNC (RiscVSetSupervisorScratch)
    csrrw a1, CSR_SSCRATCH, a0
    ret

//
// Get Supervisor mode scratch.
// @retval a0 : Value in Supervisor mode scratch
//
ASM_FUNC (RiscVGetSupervisorScratch)
    csrr a0, CSR_SSCRATCH
    ret

//
// Set Supervisor mode trap vector.
// @param a0 : Value set to Supervisor mode trap vector
//
ASM_FUNC (RiscVSetSupervisorStvec)
    csrrw a1, CSR_STVEC, a0
    ret

//
// Get Supervisor mode trap vector.
// @retval a0 : Value in Supervisor mode trap vector
//
ASM_FUNC (RiscVGetSupervisorStvec)
    csrr a0, CSR_STVEC
    ret

//
// Get Supervisor trap cause CSR.
//
ASM_FUNC (RiscVGetSupervisorTrapCause)
    csrrs a0, CSR_SCAUSE, 0
    ret

//
// Set Supervisor Address Translation and
// Protection Register.
//
ASM_FUNC (RiscVSetSupervisorAddressTranslationRegister)
    csrw  CSR_SATP, a0
    ret

//
// Read TIME CSR.
// @retval a0 : 64-bit timer.
//
ASM_FUNC (RiscVReadTimer)
    csrr a0, CSR_TIME
    ret
