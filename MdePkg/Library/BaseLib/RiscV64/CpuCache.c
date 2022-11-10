/** @file
  CPU pause for RISC-V

  Copyright (c) 2020, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2022, Rivos Inc. All rights reserved.<BR>


  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "BaseLibInternals.h"

extern VOID
RiscVCpuCacheFlush (
  UINTN Address
  );

/**
  Requests CPU to pause for a short period of time.

  Requests CPU to pause for a short period of time. Typically used in MP
  systems to prevent memory starvation while waiting for a spin lock.

**/
VOID
EFIAPI
CpuCacheFlush (
  UINTN Address
  )
{
  RiscVCpuCacheFlush (Address);
}

extern VOID
RiscVCpuCacheClean (
  UINTN Address
  );

/**
  Requests CPU to pause for a short period of time.

  Requests CPU to pause for a short period of time. Typically used in MP
  systems to prevent memory starvation while waiting for a spin lock.

**/
VOID
EFIAPI
CpuCacheClean (
  UINTN Address
  )
{
  RiscVCpuCacheClean (Address);
}

extern VOID
RiscVCpuCacheInvd (
  UINTN Address
  );

/**
  Requests CPU to pause for a short period of time.

  Requests CPU to pause for a short period of time. Typically used in MP
  systems to prevent memory starvation while waiting for a spin lock.

**/
VOID
EFIAPI
CpuCacheInvd (
  UINTN Address
  )
{
  RiscVCpuCacheInvd (Address);
}
