## @file
#  Definitions of Flash definition file on RiscVVirt RISC-V platform
#
#  Copyright (c) 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
#  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
[Defines]
DEFINE BLOCK_SIZE        = 0x1000

DEFINE FW_BASE_ADDRESS   = 0x22000000
DEFINE FW_SIZE           = 0x00300000
DEFINE FW_BLOCKS         = 0x300

DEFINE CODE_BASE_ADDRESS = $(FW_BASE_ADDRESS)
DEFINE CODE_SIZE         = 0x00240000
DEFINE CODE_BLOCKS       = 0x240


#
# The size of memory region must be power of 2.
# The base address must be aligned with the size.
#
# FW memory region
#
DEFINE SECFV_OFFSET                  = 0x00000000
DEFINE SECFV_SIZE                    = 0x00040000
DEFINE FVMAIN_OFFSET                 = 0x00040000
DEFINE FVMAIN_SIZE                   = 0x00200000

#
# EFI Variable memory region.
# The total size of EFI Variable FD must include
# all of sub regions of EFI Variable
#
DEFINE VARS_OFFSET                   = 0x00240000
DEFINE VARS_SIZE                     = 0x00040000
DEFINE VARS_FTW_WORKING_OFFSET       = 0x00280000
DEFINE VARS_FTW_WORKING_SIZE         = 0x00040000
DEFINE VARS_FTW_SPARE_OFFSET         = 0x002c0000
DEFINE VARS_FTW_SPARE_SIZE           = 0x00040000

#
# Base Address where SEC phase will decompress and load
# the PEI and DXE FVs
#
DEFINE MEMFD_BASE_ADDRESS            = 0x80200000
