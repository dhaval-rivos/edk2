/** @file
  Master header file for SecCore.

  Copyright (c) 2019-2022, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SEC_MAIN_H_
#define _SEC_MAIN_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/DebugLib.h>
#include <Library/ExtractGuidedSectionLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeCoffExtraActionLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/PeCoffLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/RiscVCpuLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/PrintLib.h>
#include <Library/RiscVSbiLib.h>
#include <Register/RiscV64/RiscVImpl.h>
#include <Ppi/TemporaryRamDone.h>
#include <Ppi/TemporaryRamSupport.h>

/**
  Entry point to the C language phase of SEC. After the SEC assembly
  code has initialized some temporary memory and set up the stack,
  the control is transferred to this function.

  @param SizeOfRam           Size of the temporary memory available for use.
  @param TempRamBase         Base address of temporary ram
  @param BootFirmwareVolume  Base address of the Boot Firmware Volume.
**/
VOID
NORETURN
EFIAPI
SecStartup (
  IN  UINTN BootHartId,
  IN  VOID *DeviceTreeAddress
  );

/**
  Auto-generated function that calls the library constructors for all of the module's
  dependent libraries.  This function must be called by the SEC Core once a stack has
  been established.

**/
VOID
EFIAPI
ProcessLibraryConstructorList (
  VOID
  );

#endif
