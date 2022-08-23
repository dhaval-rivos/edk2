/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __RISCV_ENCODING_H__
#define __RISCV_ENCODING_H__

#include <Register/RiscV64/RiscVConst.h>

/* clang-format off */
#define MSTATUS_SIE                     _UL(0x00000002)
#define MSTATUS_MIE                     _UL(0x00000008)
#define MSTATUS_SPIE_SHIFT              5
#define MSTATUS_SPIE                    (_UL(1) << MSTATUS_SPIE_SHIFT)
#define MSTATUS_UBE                     _UL(0x00000040)
#define MSTATUS_MPIE                    _UL(0x00000080)
#define MSTATUS_SPP_SHIFT               8
#define MSTATUS_SPP                     (_UL(1) << MSTATUS_SPP_SHIFT)
#define MSTATUS_MPP_SHIFT               11
#define MSTATUS_MPP                     (_UL(3) << MSTATUS_MPP_SHIFT)

#define SSTATUS_SIE                     MSTATUS_SIE
#define SSTATUS_SPIE_SHIFT              MSTATUS_SPIE_SHIFT
#define SSTATUS_SPIE                    MSTATUS_SPIE
#define SSTATUS_SPP_SHIFT               MSTATUS_SPP_SHIFT
#define SSTATUS_SPP                     MSTATUS_SPP

#define IRQ_S_SOFT                      1
#define IRQ_VS_SOFT                     2
#define IRQ_M_SOFT                      3
#define IRQ_S_TIMER                     5
#define IRQ_VS_TIMER                    6
#define IRQ_M_TIMER                     7
#define IRQ_S_EXT                       9
#define IRQ_VS_EXT                      10
#define IRQ_M_EXT                       11
#define IRQ_S_GEXT                      12
#define IRQ_PMU_OVF                     13

#define MIP_SSIP                        (_UL(1) << IRQ_S_SOFT)
#define MIP_VSSIP                       (_UL(1) << IRQ_VS_SOFT)
#define MIP_MSIP                        (_UL(1) << IRQ_M_SOFT)
#define MIP_STIP                        (_UL(1) << IRQ_S_TIMER)
#define MIP_VSTIP                       (_UL(1) << IRQ_VS_TIMER)
#define MIP_MTIP                        (_UL(1) << IRQ_M_TIMER)
#define MIP_SEIP                        (_UL(1) << IRQ_S_EXT)
#define MIP_VSEIP                       (_UL(1) << IRQ_VS_EXT)
#define MIP_MEIP                        (_UL(1) << IRQ_M_EXT)
#define MIP_SGEIP                       (_UL(1) << IRQ_S_GEXT)
#define MIP_LCOFIP                      (_UL(1) << IRQ_PMU_OVF)

#define SIP_SSIP                        MIP_SSIP
#define SIP_STIP                        MIP_STIP

#define PRV_U                           _UL(0)
#define PRV_S                           _UL(1)
#define PRV_M                           _UL(3)

#define SATP64_MODE                     _ULL(0xF000000000000000)
#define SATP64_ASID                     _ULL(0x0FFFF00000000000)
#define SATP64_PPN                      _ULL(0x00000FFFFFFFFFFF)

#define SATP_MODE_OFF                   _UL(0)
#define SATP_MODE_SV32                  _UL(1)
#define SATP_MODE_SV39                  _UL(8)
#define SATP_MODE_SV48                  _UL(9)
#define SATP_MODE_SV57                  _UL(10)
#define SATP_MODE_SV64                  _UL(11)


#define SATP_MODE                       SATP64_MODE

/* ===== User-level CSRs ===== */

/* User Counters/Timers */
#define CSR_CYCLE                       0xc00
#define CSR_TIME                        0xc01

/* ===== Supervisor-level CSRs ===== */

/* Supervisor Trap Setup */
#define CSR_SSTATUS                     0x100
#define CSR_SEDELEG                     0x102
#define CSR_SIDELEG                     0x103
#define CSR_SIE                         0x104
#define CSR_STVEC                       0x105

/* Supervisor Configuration */
#define CSR_SENVCFG                     0x10a

/* Supervisor Trap Handling */
#define CSR_SSCRATCH                    0x140
#define CSR_SEPC                        0x141
#define CSR_SCAUSE                      0x142
#define CSR_STVAL                       0x143
#define CSR_SIP                         0x144

/* Supervisor Protection and Translation */
#define CSR_SATP                        0x180

/* ===== Trap/Exception Causes ===== */

#define CAUSE_MISALIGNED_FETCH          0x0
#define CAUSE_FETCH_ACCESS              0x1
#define CAUSE_ILLEGAL_INSTRUCTION       0x2
#define CAUSE_BREAKPOINT                0x3
#define CAUSE_MISALIGNED_LOAD           0x4
#define CAUSE_LOAD_ACCESS               0x5
#define CAUSE_MISALIGNED_STORE          0x6
#define CAUSE_STORE_ACCESS              0x7
#define CAUSE_USER_ECALL                0x8
#define CAUSE_SUPERVISOR_ECALL          0x9
#define CAUSE_VIRTUAL_SUPERVISOR_ECALL  0xa
#define CAUSE_MACHINE_ECALL             0xb
#define CAUSE_FETCH_PAGE_FAULT          0xc
#define CAUSE_LOAD_PAGE_FAULT           0xd
#define CAUSE_STORE_PAGE_FAULT          0xf
#define CAUSE_FETCH_GUEST_PAGE_FAULT    0x14
#define CAUSE_LOAD_GUEST_PAGE_FAULT     0x15
#define CAUSE_VIRTUAL_INST_FAULT        0x16
#define CAUSE_STORE_GUEST_PAGE_FAULT    0x17

#endif
