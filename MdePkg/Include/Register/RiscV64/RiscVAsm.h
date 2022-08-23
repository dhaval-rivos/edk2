/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __RISCV_ASM_H__
#define __RISCV_ASM_H__

#include <Register/RiscV64/RiscVEncoding.h>

#ifdef __ASSEMBLER__
#define __ASM_STR(x)    x
#else
#define __ASM_STR(x)    #x
#endif

#ifndef __ASSEMBLER__

#define csr_swap(csr, val)                                  \
  ({                                                        \
    unsigned long __v = (unsigned long)(val);               \
    __asm__ __volatile__("csrrw %0, " __ASM_STR(csr) ", %1" \
                         : "=r"(__v)                        \
                         : "rK"(__v)                        \
                         : "memory");                       \
    __v;                                                    \
  })

#define csr_read(csr)                               \
  ({                                                \
    register unsigned long __v;                     \
    __asm__ __volatile__("csrr %0, " __ASM_STR(csr) \
                         : "=r"(__v)                \
                         :                          \
                         : "memory");               \
    __v;                                            \
  })

#define csr_write(csr, val)                            \
  ({                                                   \
    unsigned long __v = (unsigned long)(val);          \
    __asm__ __volatile__("csrw " __ASM_STR(csr) ", %0" \
                         :                             \
                         : "rK"(__v)                   \
                         : "memory");                  \
  })

#define csr_read_set(csr, val)                              \
  ({                                                        \
    unsigned long __v = (unsigned long)(val);               \
    __asm__ __volatile__("csrrs %0, " __ASM_STR(csr) ", %1" \
                         : "=r"(__v)                        \
                         : "rK"(__v)                        \
                         : "memory");                       \
    __v;                                                    \
  })

#define csr_set(csr, val)                              \
  ({                                                   \
    unsigned long __v = (unsigned long)(val);          \
    __asm__ __volatile__("csrs " __ASM_STR(csr) ", %0" \
                         :                             \
                         : "rK"(__v)                   \
                         : "memory");                  \
  })

#define csr_read_clear(csr, val)                            \
  ({                                                        \
    unsigned long __v = (unsigned long)(val);               \
    __asm__ __volatile__("csrrc %0, " __ASM_STR(csr) ", %1" \
                         : "=r"(__v)                        \
                         : "rK"(__v)                        \
                         : "memory");                       \
    __v;                                                    \
  })

#define csr_clear(csr, val)                            \
  ({                                                   \
    unsigned long __v = (unsigned long)(val);          \
    __asm__ __volatile__("csrc " __ASM_STR(csr) ", %0" \
                         :                             \
                         : "rK"(__v)                   \
                         : "memory");                  \
  })

#define wfi()                                 \
  do {                                        \
    __asm__ __volatile__("wfi" ::: "memory"); \
  } while (0)

#define ebreak()                                 \
  do {                                           \
    __asm__ __volatile__("ebreak" ::: "memory"); \
  } while (0)


#endif /* !__ASSEMBLER__ */

#endif
