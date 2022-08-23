/*
  Copyright (c) 2021-2022 , Hewlett Packard Enterprise Development LP. All rights reserved.
  Copyright (c) 2019 Western Digital Corporation or its affiliates.
  Copyright (c) 2022 Ventana Micro Systems Inc.

  SPDX-License-Identifier: BSD-2-Clause-Patent

 */

#include "SecMain.h"

.text
.align 3

ASM_FUNC (_ModuleEntryPoint)
  /* Use Temp memory as the stack for calling to C code */
  li    a4, FixedPcdGet32 (PcdTemporaryRamBase)
  li    a5, FixedPcdGet32 (PcdTemporaryRamSize)

  /* Use Temp memory as the stack for calling to C code */
  add   sp, a4, a5

  call SecStartup
