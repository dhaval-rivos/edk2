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
#include <Register/RiscV64/RiscVAsm.h>
#include <Register/RiscV64/RiscVImpl.h>

.data

.text
.align 3

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
