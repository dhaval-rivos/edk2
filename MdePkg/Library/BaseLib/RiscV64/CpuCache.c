/** @file
  CPU Cache Operations for RISC-V

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
   Flush CPU Cacheline as per RV Zicbo spec
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
   Clean CPU Cacheline as per RV Zicbo spec
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
   Invd CPU Cacheline as per RV Zicbo spec
**/
VOID
EFIAPI
CpuCacheInvd (
  UINTN Address
  )
{
  RiscVCpuCacheInvd (Address);
}
