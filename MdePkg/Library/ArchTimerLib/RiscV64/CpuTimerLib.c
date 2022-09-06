/** @file
  RISC-V instance of Timer Library.

  Copyright (c) 2016 - 2022, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Guid/FdtHob.h>
#include <Register/RiscV64/RiscVAsm.h>
#include <Register/RiscV64/RiscVImpl.h>
#include <libfdt.h>


STATIC UINT32 mTimeBaseFrequency;
STATIC BOOLEAN mTimeBaseFreqInitialized;

UINT32
InternalGetTimerFrequency(
  VOID
  )
{
  return mTimeBaseFrequency;
}


/**
  Stalls the CPU for at least the given number of ticks.

  Stalls the CPU for at least the given number of ticks. It's invoked by
  MicroSecondDelay() and NanoSecondDelay().

  @param  Delay     A period of time to delay in ticks.

**/
VOID
InternalRiscVTimerDelay (
  IN UINT32  Delay
  )
{
  UINT32  Ticks;
  UINT32  Times;

  Times  = Delay >> (RISCV_TIMER_COMPARE_BITS - 2);
  Delay &= ((1 << (RISCV_TIMER_COMPARE_BITS - 2)) - 1);
  do {
    //
    // The target timer count is calculated here
    //
    Ticks = csr_read(CSR_TIME) + Delay;
    Delay = 1 << (RISCV_TIMER_COMPARE_BITS - 2);
    while (((Ticks - csr_read(CSR_TIME)) & (1 << (RISCV_TIMER_COMPARE_BITS - 1))) == 0) {
      CpuPause ();
    }
  } while (Times-- > 0);
}

/**
  Stalls the CPU for at least the given number of microseconds.

  Stalls the CPU for the number of microseconds specified by MicroSeconds.

  @param  MicroSeconds  The minimum number of microseconds to delay.

  @return MicroSeconds

**/
UINTN
EFIAPI
MicroSecondDelay (
  IN UINTN  MicroSeconds
  )
{
  InternalRiscVTimerDelay (
    (UINT32)DivU64x32 (
              MultU64x32 (
                MicroSeconds,
                InternalGetTimerFrequency()
                ),
              1000000u
              )
    );
  return MicroSeconds;
}

/**
  Stalls the CPU for at least the given number of nanoseconds.

  Stalls the CPU for the number of nanoseconds specified by NanoSeconds.

  @param  NanoSeconds The minimum number of nanoseconds to delay.

  @return NanoSeconds

**/
UINTN
EFIAPI
NanoSecondDelay (
  IN UINTN  NanoSeconds
  )
{
  InternalRiscVTimerDelay (
    (UINT32)DivU64x32 (
              MultU64x32 (
                NanoSeconds,
                InternalGetTimerFrequency()
                ),
              1000000000u
              )
    );
  return NanoSeconds;
}

/**
  Retrieves the current value of a 64-bit free running performance counter.

  Retrieves the current value of a 64-bit free running performance counter. The
  counter can either count up by 1 or count down by 1. If the physical
  performance counter counts by a larger increment, then the counter values
  must be translated. The properties of the counter can be retrieved from
  GetPerformanceCounterProperties().

  @return The current value of the free running performance counter.

**/
UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
  return (UINT64)csr_read (CSR_TIME);
}

/**return
  Retrieves the 64-bit frequency in Hz and the range of performance counter
  values.

  If StartValue is not NULL, then the value that the performance counter starts
  with immediately after is it rolls over is returned in StartValue. If
  EndValue is not NULL, then the value that the performance counter end with
  immediately before it rolls over is returned in EndValue. The 64-bit
  frequency of the performance counter in Hz is always returned. If StartValue
  is less than EndValue, then the performance counter counts up. If StartValue
  is greater than EndValue, then the performance counter counts down. For
  example, a 64-bit free running counter that counts up would have a StartValue
  of 0 and an EndValue of 0xFFFFFFFFFFFFFFFF. A 24-bit free running counter
  that counts down would have a StartValue of 0xFFFFFF and an EndValue of 0.

  @param  StartValue  The value the performance counter starts with when it
                      rolls over.
  @param  EndValue    The value that the performance counter ends with before
                      it rolls over.

  @return The frequency in Hz.

**/
UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT      UINT64 *StartValue, OPTIONAL
  OUT      UINT64                    *EndValue     OPTIONAL
  )
{
  if (StartValue != NULL) {
    *StartValue = 0;
  }

  if (EndValue != NULL) {
    *EndValue = 32 - 1;
  }

  return InternalGetTimerFrequency();
}

/**
  Converts elapsed ticks of performance counter to time in nanoseconds.

  This function converts the elapsed ticks of running performance counter to
  time value in unit of nanoseconds.

  @param  Ticks     The number of elapsed ticks of running performance counter.

  @return The elapsed time in nanoseconds.

**/
UINT64
EFIAPI
GetTimeInNanoSecond (
  IN      UINT64  Ticks
  )
{
  UINT64  NanoSeconds;
  UINT32  Remainder;

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64x32 (DivU64x32Remainder (Ticks, InternalGetTimerFrequency(), &Remainder), 1000000000u);

  //
  // Frequency < 0x100000000, so Remainder < 0x100000000, then (Remainder * 1,000,000,000)
  // will not overflow 64-bit.
  //
  NanoSeconds += DivU64x32 (MultU64x32 ((UINT64)Remainder, 1000000000u), InternalGetTimerFrequency());

  return NanoSeconds;
}

STATIC
RETURN_STATUS
EFIAPI
FdtInitializeTimerFrequency (
  VOID
  )
{
  VOID            *Hob;
  VOID            *Fdt;
  INT32           CpusNode, Len;
  const fdt32_t   *Prop;

  Hob = GetFirstGuidHob (&gFdtHobGuid);
  if ((Hob == NULL) || (GET_GUID_HOB_DATA_SIZE (Hob) != sizeof (UINT64))) {
    DEBUG ((DEBUG_ERROR, "%a: No FDT Hob found\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  Fdt = (VOID *)(UINTN)*(UINT64 *)GET_GUID_HOB_DATA (Hob);

  if (fdt_check_header (Fdt) != 0) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: No DTB found @ 0x%p\n",
      __FUNCTION__,
      Fdt
      ));
    return EFI_NOT_FOUND;
  }

  // The "cpus" node resides at the the root of the DT. Fetch it.
  CpusNode = fdt_path_offset (Fdt, "/cpus");
  if (CpusNode < 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid /cpus node\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  Prop = fdt_getprop((void *)Fdt, CpusNode, "timebase-frequency", &Len);
  if (!Prop) {
    DEBUG ((DEBUG_ERROR, "%a: timebase-frequency propertynot found\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  mTimeBaseFrequency = fdt32_to_cpu(*Prop);
  DEBUG((DEBUG_INFO, "%a: Timer Frequency (DT) is set to 0x%x\n", __FUNCTION__, mTimeBaseFrequency));

  return EFI_SUCCESS;
}
/**
  Initializes the Timer Frequency by reading it from the DTB

**/
RETURN_STATUS
EFIAPI
ArchTimerLibConstructor (
  VOID
  )
{
  EFI_STATUS      Status;

  /*
   * Initialize only once
   */
  if (mTimeBaseFreqInitialized) {
    return EFI_SUCCESS;
  }

  mTimeBaseFreqInitialized = 1;

  Status = FdtInitializeTimerFrequency();

  if (EFI_ERROR(Status)) {
    mTimeBaseFrequency = PcdGet32 (PcdRiscVTimerFrequencyInHz);
    DEBUG((DEBUG_INFO, "%a: Timer Frequency (PCD) is set to 0x%x\n", __FUNCTION__, mTimeBaseFrequency));
  }

  return EFI_SUCCESS;
}
