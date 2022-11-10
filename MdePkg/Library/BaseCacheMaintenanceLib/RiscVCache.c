/** @file
  RISC-V specific functionality for cache.

  Copyright (c) 2020, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2022, Rivos Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

/**
  Use runtime discovery mechanism in future when avalable
  through https://lists.riscv.org/g/tech-privileged/topic/83853282
**/
#define RV64_CACHE_BLOCK_SIZE   64

typedef enum{
  cln,
  flsh,
  invd,
}CACHE_OP;

/* Ideally we should do this through BaseLib.h by adding
   Asm*CacheLine functions. This can be done after Initial
   RV refactoring is complete. For now call functions directly
*/
VOID
EFIAPI RiscVCpuCacheFlush (
  UINTN
  );

VOID
EFIAPI RiscVCpuCacheClean (
  UINTN
  );

VOID
EFIAPI RiscVCpuCacheInval (
  UINTN
  );


/**
  Performs required opeartion on cache lines in the cache coherency domain
  of the calling CPU. If Address is not aligned on a cache line boundary, 
  then entire cache line containing Address is operated. If Address + Length
  is not aligned on a cache line boundary, then the entire cache line 
  containing Address + Length -1 is operated.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the cache lines to
                  invalidate. If the CPU is in a physical addressing mode, then
                  Address is a physical address. If the CPU is in a virtual
                  addressing mode, then Address is a virtual address.

  @param  Length  The number of bytes to invalidate from the instruction cache.

  @return Address.

**/

VOID *
EFIAPI
CacheOpCacheRange (
  IN VOID   *Address,
  IN UINTN  Length,
  IN CACHE_OP op
  )
{
  UINTN   CacheLineSize;
  UINTN   Start;
  UINTN   End;

  if (Length == 0) {
    return Address;
  }

  ASSERT ((Length - 1) <= (MAX_ADDRESS - (UINTN)Address));

  //
  // Cache line size is 8 * Bits 15-08 of EBX returned from CPUID 01H
  //
  CacheLineSize = RV64_CACHE_BLOCK_SIZE;

  Start = (UINTN)Address;
  //
  // Calculate the cache line alignment
  //
  End    = (Start + Length + (CacheLineSize - 1)) & ~(CacheLineSize - 1);
  Start &= ~((UINTN)CacheLineSize - 1);

  do {
    switch (op) {
      case invd:
        RiscVCpuCacheInval(Start);
        break;
      case flsh:
        RiscVCpuCacheFlush(Start);
        break;
      case cln:
        RiscVCpuCacheClean(Start);
        break;
      default:
        DEBUG ((DEBUG_ERROR, "%a:RISC-V unsupported operation\n"));
        break;
    }

    Start = Start + CacheLineSize;
  } while (Start != End);

  return Address;
}

/**
  RV does not support whole cache invd functionality.
  There maybe platform level implementation which is
  outside of this implementation.
**/

VOID
EFIAPI
InvalidateInstructionCache (
  VOID
  )
{
  DEBUG ((DEBUG_ERROR, "%a:RISC-V unsupported function.\n", __FUNCTION__));
}


/**
  An invalidate operation makes data from store operations performed by a
  set of non-coherent agents visible to the set of coherent agents at a
  point common to both sets by deallocating all copies of a cache block
  from the set of coherent caches up to that point.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the instruction cache lines to
                  invalidate. If the CPU is in a physical addressing mode, then
                  Address is a physical address. If the CPU is in a virtual
                  addressing mode, then Address is a virtual address.

  @param  Length  The number of bytes to invalidate from the instruction cache.

  @return Address.

**/
VOID *
EFIAPI
InvalidateInstructionCacheRange (
  IN VOID   *Address,
  IN UINTN  Length
  )
{
  //RV does not support $I specific operation.
  CacheOpCacheRange(Address, Length, invd);
  return Address;
}

/**
  RV does not support whole cache invd functionality.
  There maybe platform level implementation which is
  outside of this implementation.
**/

VOID
EFIAPI
InvalidateDataCache (
  VOID
  )
{
  DEBUG ((DEBUG_ERROR, "%a:RISC-V unsupported function.\n", __FUNCTION__));
}

/**
  Invalidates a range of data cache lines in the cache coherency domain of the
  calling CPU.

  Invalidates the data cache lines specified by Address and Length. If Address
  is not aligned on a cache line boundary, then entire data cache line
  containing Address is invalidated. If Address + Length is not aligned on a
  cache line boundary, then the entire data cache line containing Address +
  Length -1 is invalidated. This function must never invalidate any cache lines
  outside the specified range. If Length is 0, then no data cache lines are
  invalidated. Address is returned. This function must be used with care
  because dirty cache lines are not written back to system memory. It is
  typically used for cache diagnostics. If the CPU does not support
  invalidation of a data cache range, then a write back and invalidate
  operation should be performed on the data cache range.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the data cache lines to invalidate. If
                  the CPU is in a physical addressing mode, then Address is a
                  physical address. If the CPU is in a virtual addressing mode,
                  then Address is a virtual address.
  @param  Length  The number of bytes to invalidate from the data cache.

  @return Address.

**/
VOID *
EFIAPI
InvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  //RV does not support $D specific operation.
  CacheOpCacheRange(Address, Length, invd);
  return Address;
}

/**
  RV does not support whole cache Invd functionality.
  There maybe platform level implementation which is
  outside of this implementation.
**/

VOID
EFIAPI
WriteBackInvalidateDataCache (
  VOID
  )
{
  DEBUG ((DEBUG_ERROR, "%a:RISC-V unsupported function.\n", __FUNCTION__));
}

/**
  Writes back and invalidates a range of data cache lines in the cache
  coherency domain of the calling CPU.

  Writes back and invalidates the data cache lines specified by Address and
  Length. If Address is not aligned on a cache line boundary, then entire data
  cache line containing Address is written back and invalidated. If Address +
  Length is not aligned on a cache line boundary, then the entire data cache
  line containing Address + Length -1 is written back and invalidated. This
  function may choose to write back and invalidate the entire data cache if
  that is more efficient than writing back and invalidating the specified
  range. If Length is 0, then no data cache lines are written back and
  invalidated. Address is returned.

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the data cache lines to write back and
                  invalidate. If the CPU is in a physical addressing mode, then
                  Address is a physical address. If the CPU is in a virtual
                  addressing mode, then Address is a virtual address.
  @param  Length  The number of bytes to write back and invalidate from the
                  data cache.

  @return Address of cache invalidation.

**/
VOID *
EFIAPI
WriteBackInvalidateDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  CacheOpCacheRange(Address, Length, flsh);
  return Address;
}

/**
  RV does not support whole cache WB functionality.
  There maybe platform level implementation which is
  outside of this implementation.
**/

VOID
EFIAPI
WriteBackDataCache (
  VOID
  )
{
  DEBUG ((DEBUG_ERROR, "%a:RISC-V unsupported function.\n", __FUNCTION__));
}

/**
  Writes back a range of data cache lines in the cache coherency domain of the
  calling CPU.

  Called clean operation in RV spec, it makes data from store operations
  performed by the set of coherent agents visible to a set
  of non-coherent agents at a point common to both sets by performing a write
  transfer of a copy of a cache  block to that point provided a coherent
  agent performed a store operation that modified the data in the cache block
  since the previous invalidate, clean, or flush operation on the cache block

  If Length is greater than (MAX_ADDRESS - Address + 1), then ASSERT().

  @param  Address The base address of the data cache lines to write back. If
                  the CPU is in a physical addressing mode, then Address is a
                  physical address. If the CPU is in a virtual addressing
                  mode, then Address is a virtual address.
  @param  Length  The number of bytes to write back from the data cache.

  @return Address of cache written in main memory.

**/
VOID *
EFIAPI
WriteBackDataCacheRange (
  IN      VOID   *Address,
  IN      UINTN  Length
  )
{
  CacheOpCacheRange(Address, Length, cln);
  return Address;
}

