/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __RISCV_ENCODING_H__
#define __RISCV_ENCODING_H__

/* ===== Supervisor-level CSRs ===== */

/* Supervisor Trap Setup */
#define CSR_SSTATUS			0x100
#define CSR_SEDELEG			0x102
#define CSR_SIDELEG			0x103
#define CSR_SIE				0x104
#define CSR_STVEC			0x105
#define CSR_SCOUNTEREN			0x106

/* Supervisor Configuration */
#define CSR_SENVCFG			0x10a

/* Supervisor Trap Handling */
#define CSR_SSCRATCH			0x140
#define CSR_SEPC			0x141
#define CSR_SCAUSE			0x142
#define CSR_STVAL			0x143
#define CSR_SIP				0x144

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

#define MIP_SSIP                        (1UL << IRQ_S_SOFT)
#define MIP_VSSIP                       (1UL << IRQ_VS_SOFT)
#define MIP_MSIP                        (1UL << IRQ_M_SOFT)
#define MIP_STIP                        (1UL) << IRQ_S_TIMER
#define MIP_VSTIP                       (1UL << IRQ_VS_TIMER)
#define MIP_MTIP                        (1UL << IRQ_M_TIMER)
#define MIP_SEIP                        (1UL << IRQ_S_EXT)
#define MIP_VSEIP                       (1UL << IRQ_VS_EXT)
#define MIP_MEIP                        (1UL << IRQ_M_EXT
#define MIP_SGEIP                       (1UL << IRQ_S_GEXT)

#define SIP_SSIP                        MIP_SSIP
#define SIP_STIP                        MIP_STIP

#endif
