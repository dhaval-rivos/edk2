/** @file
  Library to call the RISC-V SBI ecalls

  Copyright (c) 2021-2022, Hewlett Packard Development LP. All rights reserved.<BR>
  Copyright (c) 2022, Ventana Micro Systems Inc. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Hart - Hardware Thread, similar to a CPU core
**/

#ifndef RISCV_SBI_LIB_H_
#define RISCV_SBI_LIB_H_

#include <Uefi.h>

/* SBI Extension IDs */
#define SBI_EXT_0_1_SET_TIMER			0x0
#define SBI_EXT_0_1_CONSOLE_PUTCHAR		0x1
#define SBI_EXT_0_1_CONSOLE_GETCHAR		0x2
#define SBI_EXT_0_1_CLEAR_IPI			0x3
#define SBI_EXT_0_1_SEND_IPI			0x4
#define SBI_EXT_0_1_REMOTE_FENCE_I		0x5
#define SBI_EXT_0_1_REMOTE_SFENCE_VMA		0x6
#define SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID	0x7
#define SBI_EXT_0_1_SHUTDOWN			0x8
#define SBI_EXT_BASE				0x10
#define SBI_EXT_TIME				0x54494D45
#define SBI_EXT_IPI				0x735049
#define SBI_EXT_RFENCE				0x52464E43
#define SBI_EXT_HSM				0x48534D
#define SBI_EXT_SRST				0x53525354
#define SBI_EXT_PMU				0x504D55

/* SBI function IDs for BASE extension*/
#define SBI_EXT_BASE_GET_SPEC_VERSION		0x0
#define SBI_EXT_BASE_GET_IMP_ID			0x1
#define SBI_EXT_BASE_GET_IMP_VERSION		0x2
#define SBI_EXT_BASE_PROBE_EXT			0x3
#define SBI_EXT_BASE_GET_MVENDORID		0x4
#define SBI_EXT_BASE_GET_MARCHID		0x5
#define SBI_EXT_BASE_GET_MIMPID			0x6

/* SBI function IDs for TIME extension*/
#define SBI_EXT_TIME_SET_TIMER			0x0

/* SBI function IDs for IPI extension*/
#define SBI_EXT_IPI_SEND_IPI			0x0

/* SBI function IDs for RFENCE extension*/
#define SBI_EXT_RFENCE_REMOTE_FENCE_I		0x0
#define SBI_EXT_RFENCE_REMOTE_SFENCE_VMA	0x1
#define SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID	0x2
#define SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID	0x3
#define SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA	0x4
#define SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID	0x5
#define SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA	0x6

/* SBI function IDs for HSM extension */
#define SBI_EXT_HSM_HART_START			0x0
#define SBI_EXT_HSM_HART_STOP			0x1
#define SBI_EXT_HSM_HART_GET_STATUS		0x2
#define SBI_EXT_HSM_HART_SUSPEND		0x3

#define SBI_HSM_STATE_STARTED			0x0
#define SBI_HSM_STATE_STOPPED			0x1
#define SBI_HSM_STATE_START_PENDING		0x2
#define SBI_HSM_STATE_STOP_PENDING		0x3
#define SBI_HSM_STATE_SUSPENDED			0x4
#define SBI_HSM_STATE_SUSPEND_PENDING		0x5
#define SBI_HSM_STATE_RESUME_PENDING		0x6

#define SBI_HSM_SUSP_BASE_MASK			0x7fffffff
#define SBI_HSM_SUSP_NON_RET_BIT		0x80000000
#define SBI_HSM_SUSP_PLAT_BASE			0x10000000

#define SBI_HSM_SUSPEND_RET_DEFAULT		0x00000000
#define SBI_HSM_SUSPEND_RET_PLATFORM		SBI_HSM_SUSP_PLAT_BASE
#define SBI_HSM_SUSPEND_RET_LAST		SBI_HSM_SUSP_BASE_MASK
#define SBI_HSM_SUSPEND_NON_RET_DEFAULT		SBI_HSM_SUSP_NON_RET_BIT
#define SBI_HSM_SUSPEND_NON_RET_PLATFORM	(SBI_HSM_SUSP_NON_RET_BIT | \
						 SBI_HSM_SUSP_PLAT_BASE)
#define SBI_HSM_SUSPEND_NON_RET_LAST		(SBI_HSM_SUSP_NON_RET_BIT | \
						 SBI_HSM_SUSP_BASE_MASK)

/* SBI function IDs for SRST extension */
#define SBI_EXT_SRST_RESET			0x0

#define SBI_SRST_RESET_TYPE_SHUTDOWN		0x0
#define SBI_SRST_RESET_TYPE_COLD_REBOOT	0x1
#define SBI_SRST_RESET_TYPE_WARM_REBOOT	0x2
#define SBI_SRST_RESET_TYPE_LAST	SBI_SRST_RESET_TYPE_WARM_REBOOT

#define SBI_SRST_RESET_REASON_NONE	0x0
#define SBI_SRST_RESET_REASON_SYSFAIL	0x1

/* SBI function IDs for PMU extension */
#define SBI_EXT_PMU_NUM_COUNTERS	0x0
#define SBI_EXT_PMU_COUNTER_GET_INFO	0x1
#define SBI_EXT_PMU_COUNTER_CFG_MATCH	0x2
#define SBI_EXT_PMU_COUNTER_START	0x3
#define SBI_EXT_PMU_COUNTER_STOP	0x4
#define SBI_EXT_PMU_COUNTER_FW_READ	0x5

/** General pmu event codes specified in SBI PMU extension */
enum sbi_pmu_hw_generic_events_t {
	SBI_PMU_HW_NO_EVENT			= 0,
	SBI_PMU_HW_CPU_CYCLES			= 1,
	SBI_PMU_HW_INSTRUCTIONS			= 2,
	SBI_PMU_HW_CACHE_REFERENCES		= 3,
	SBI_PMU_HW_CACHE_MISSES			= 4,
	SBI_PMU_HW_BRANCH_INSTRUCTIONS		= 5,
	SBI_PMU_HW_BRANCH_MISSES		= 6,
	SBI_PMU_HW_BUS_CYCLES			= 7,
	SBI_PMU_HW_STALLED_CYCLES_FRONTEND	= 8,
	SBI_PMU_HW_STALLED_CYCLES_BACKEND	= 9,
	SBI_PMU_HW_REF_CPU_CYCLES		= 10,

	SBI_PMU_HW_GENERAL_MAX,
};

/**
 * Generalized hardware cache events:
 *
 *       { L1-D, L1-I, LLC, ITLB, DTLB, BPU, NODE } x
 *       { read, write, prefetch } x
 *       { accesses, misses }
 */
enum sbi_pmu_hw_cache_id {
	SBI_PMU_HW_CACHE_L1D		= 0,
	SBI_PMU_HW_CACHE_L1I		= 1,
	SBI_PMU_HW_CACHE_LL		= 2,
	SBI_PMU_HW_CACHE_DTLB		= 3,
	SBI_PMU_HW_CACHE_ITLB		= 4,
	SBI_PMU_HW_CACHE_BPU		= 5,
	SBI_PMU_HW_CACHE_NODE		= 6,

	SBI_PMU_HW_CACHE_MAX,
};

enum sbi_pmu_hw_cache_op_id {
	SBI_PMU_HW_CACHE_OP_READ	= 0,
	SBI_PMU_HW_CACHE_OP_WRITE	= 1,
	SBI_PMU_HW_CACHE_OP_PREFETCH	= 2,

	SBI_PMU_HW_CACHE_OP_MAX,
};

enum sbi_pmu_hw_cache_op_result_id {
	SBI_PMU_HW_CACHE_RESULT_ACCESS	= 0,
	SBI_PMU_HW_CACHE_RESULT_MISS	= 1,

	SBI_PMU_HW_CACHE_RESULT_MAX,
};

/**
 * Special "firmware" events provided by the OpenSBI, even if the hardware
 * does not support performance events. These events are encoded as a raw
 * event type in Linux kernel perf framework.
 */
enum sbi_pmu_fw_event_code_id {
	SBI_PMU_FW_MISALIGNED_LOAD	= 0,
	SBI_PMU_FW_MISALIGNED_STORE	= 1,
	SBI_PMU_FW_ACCESS_LOAD		= 2,
	SBI_PMU_FW_ACCESS_STORE		= 3,
	SBI_PMU_FW_ILLEGAL_INSN		= 4,
	SBI_PMU_FW_SET_TIMER		= 5,
	SBI_PMU_FW_IPI_SENT		= 6,
	SBI_PMU_FW_IPI_RECVD		= 7,
	SBI_PMU_FW_FENCE_I_SENT		= 8,
	SBI_PMU_FW_FENCE_I_RECVD	= 9,
	SBI_PMU_FW_SFENCE_VMA_SENT	= 10,
	SBI_PMU_FW_SFENCE_VMA_RCVD	= 11,
	SBI_PMU_FW_SFENCE_VMA_ASID_SENT	= 12,
	SBI_PMU_FW_SFENCE_VMA_ASID_RCVD	= 13,

	SBI_PMU_FW_HFENCE_GVMA_SENT	= 14,
	SBI_PMU_FW_HFENCE_GVMA_RCVD	= 15,
	SBI_PMU_FW_HFENCE_GVMA_VMID_SENT = 16,
	SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD = 17,

	SBI_PMU_FW_HFENCE_VVMA_SENT	= 18,
	SBI_PMU_FW_HFENCE_VVMA_RCVD	= 19,
	SBI_PMU_FW_HFENCE_VVMA_ASID_SENT = 20,
	SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD = 21,
	SBI_PMU_FW_MAX,
};

/** SBI PMU event idx type */
enum sbi_pmu_event_type_id {
	SBI_PMU_EVENT_TYPE_HW				= 0x0,
	SBI_PMU_EVENT_TYPE_HW_CACHE			= 0x1,
	SBI_PMU_EVENT_TYPE_HW_RAW			= 0x2,
	SBI_PMU_EVENT_TYPE_FW				= 0xf,
	SBI_PMU_EVENT_TYPE_MAX,
};

/** SBI PMU counter type */
enum sbi_pmu_ctr_type {
	SBI_PMU_CTR_TYPE_HW = 0,
	SBI_PMU_CTR_TYPE_FW,
};

/* Helper macros to decode event idx */
#define SBI_PMU_EVENT_IDX_OFFSET 20
#define SBI_PMU_EVENT_IDX_MASK 0xFFFFF
#define SBI_PMU_EVENT_IDX_CODE_MASK 0xFFFF
#define SBI_PMU_EVENT_IDX_TYPE_MASK 0xF0000
#define SBI_PMU_EVENT_RAW_IDX 0x20000

#define SBI_PMU_EVENT_IDX_INVALID 0xFFFFFFFF

/* Flags defined for config matching function */
#define SBI_PMU_CFG_FLAG_SKIP_MATCH	(1 << 0)
#define SBI_PMU_CFG_FLAG_CLEAR_VALUE	(1 << 1)
#define SBI_PMU_CFG_FLAG_AUTO_START	(1 << 2)
#define SBI_PMU_CFG_FLAG_SET_VUINH	(1 << 3)
#define SBI_PMU_CFG_FLAG_SET_VSINH	(1 << 4)
#define SBI_PMU_CFG_FLAG_SET_UINH	(1 << 5)
#define SBI_PMU_CFG_FLAG_SET_SINH	(1 << 6)
#define SBI_PMU_CFG_FLAG_SET_MINH	(1 << 7)

/* Flags defined for counter start function */
#define SBI_PMU_START_FLAG_SET_INIT_VALUE (1 << 0)

/* Flags defined for counter stop function */
#define SBI_PMU_STOP_FLAG_RESET (1 << 0)

/* SBI base specification related macros */
#define SBI_SPEC_VERSION_MAJOR_OFFSET		24
#define SBI_SPEC_VERSION_MAJOR_MASK		0x7f
#define SBI_SPEC_VERSION_MINOR_MASK		0xffffff
#define SBI_EXT_VENDOR_START			0x09000000
#define SBI_EXT_VENDOR_END			0x09FFFFFF
#define SBI_EXT_FIRMWARE_START			0x0A000000
#define SBI_EXT_FIRMWARE_END			0x0AFFFFFF

/* SBI return error codes */
#define SBI_SUCCESS				0
#define SBI_ERR_FAILED				-1
#define SBI_ERR_NOT_SUPPORTED			-2
#define SBI_ERR_INVALID_PARAM			-3
#define SBI_ERR_DENIED				-4
#define SBI_ERR_INVALID_ADDRESS			-5
#define SBI_ERR_ALREADY_AVAILABLE		-6
#define SBI_ERR_ALREADY_STARTED			-7
#define SBI_ERR_ALREADY_STOPPED			-8

#define SBI_LAST_ERR				SBI_ERR_ALREADY_STOPPED

typedef struct {
  UINT64 BootHartId;
  VOID   *PeiServiceTable;    // PEI Service table
  UINT64 FlattenedDeviceTree; // Pointer to Flattened Device tree
} EFI_RISCV_FIRMWARE_CONTEXT;

//
// EDK2 OpenSBI Firmware extension.
//
#define SBI_EDK2_FW_EXT  (SBI_EXT_FIRMWARE_START | SBI_OPENSBI_IMPID)
//
// EDK2 OpenSBI Firmware extension functions.
//
#define SBI_EXT_FW_MSCRATCH_FUNC         0
#define SBI_EXT_FW_MSCRATCH_HARTID_FUNC  1

//
// EDK2 OpenSBI firmware extension return status.
//
typedef struct {
  UINTN    Error; ///< SBI status code
  UINTN    Value; ///< Value returned
} SBI_RET;

/**
  Get the implemented SBI specification version

  The minor number of the SBI specification is encoded in the low 24 bits,
  with the major number encoded in the next 7 bits.  Bit 32 must be 0 and is
  reserved for future expansion.

  @param[out] SpecVersion          The Version of the SBI specification.
**/
VOID
EFIAPI
SbiGetSpecVersion (
  OUT UINTN  *SpecVersion
  );

/**
  Get the SBI implementation ID

  This ID is used to identify a specific SBI implementation in order to work
  around any quirks it might have.

  @param[out] ImplId               The ID of the SBI implementation.
**/
VOID
EFIAPI
SbiGetImplId (
  OUT UINTN  *ImplId
  );

/**
  Get the SBI implementation version

  The version of this SBI implementation.
  The encoding of this number is determined by the specific SBI implementation.

  @param[out] ImplVersion          The version of the SBI implementation.
**/
VOID
EFIAPI
SbiGetImplVersion (
  OUT UINTN  *ImplVersion
  );

/**
  Probe whether an SBI extension is available

  ProbeResult is set to 0 if the extension is not available or to an extension
  specified value if it is available.

  @param[in]  ExtensionId          The extension ID.
  @param[out] ProbeResult          The return value of the probe.
**/
VOID
EFIAPI
SbiProbeExtension (
  IN  INTN  ExtensionId,
  OUT INTN  *ProbeResult
  );

/**
  Get the CPU's vendor ID

  Reads the mvendorid CSR.

  @param[out] MachineVendorId      The CPU's vendor ID.
**/
VOID
EFIAPI
SbiGetMachineVendorId (
  OUT UINTN  *MachineVendorId
  );

/**
  Get the CPU's architecture ID

  Reads the marchid CSR.

  @param[out] MachineArchId        The CPU's architecture ID.
**/
VOID
EFIAPI
SbiGetMachineArchId (
  OUT UINTN  *MachineArchId
  );

/**
  Get the CPU's implementation ID

  Reads the mimpid CSR.

  @param[out] MachineImplId        The CPU's implementation ID.
**/
VOID
EFIAPI
SbiGetMachineImplId (
  OUT UINTN  *MachineImplId
  );

/**
  Politely ask the SBI to start a given hart.

  This call may return before the hart has actually started executing, if the
  SBI implementation can guarantee that the hart is actually going to start.

  Before the hart jumps to StartAddr, the hart MUST configure PMP if present
  and switch to S-mode.

  @param[in]  HartId               The id of the hart to start.
  @param[in]  StartAddr            The physical address, where the hart starts
                                   executing from.
  @param[in]  Priv                 An XLEN-bit value, which will be in register
                                   a1 when the hart starts.
  @retval EFI_SUCCESS              Hart was stopped and will start executing from StartAddr.
  @retval EFI_LOAD_ERROR           StartAddr is not valid, possibly due to following reasons:
                                    - It is not a valid physical address.
                                    - The address is prohibited by PMP to run in
                                      supervisor mode.
  @retval EFI_INVALID_PARAMETER    HartId is not a valid hart id
  @retval EFI_ALREADY_STARTED      The hart is already running.
  @retval other                    The start request failed for unknown reasons.
**/
EFI_STATUS
EFIAPI
SbiHartStart (
  IN  UINTN  HartId,
  IN  UINTN  StartAddr,
  IN  UINTN  Priv
  );

/**
  Return execution of the calling hart to SBI.

  MUST be called in S-Mode with user interrupts disabled.
  This call is not expected to return, unless a failure occurs.

  @retval     EFI_SUCCESS          Never occurs. When successful, the call does not return.
  @retval     other                Failed to stop hard for an unknown reason.
**/
EFI_STATUS
EFIAPI
SbiHartStop (
  );

/**
  Get the current status of a hart.

  Since harts can transition between states at any time, the status retrieved
  by this function may already be out of date, once it returns.

  Possible values for HartStatus are:
  0: STARTED
  1: STOPPED
  2: START_REQUEST_PENDING
  3: STOP_REQUEST_PENDING

  @param[out] HartStatus           The pointer in which the hart's status is
                                   stored.
  @retval EFI_SUCCESS              The operation succeeds.
  @retval EFI_INVALID_PARAMETER    A parameter is invalid.
**/
EFI_STATUS
EFIAPI
SbiHartGetStatus (
  IN  UINTN  HartId,
  OUT UINTN  *HartStatus
  );

///
/// Timer extension
///

/**
  Clear pending timer interrupt bit and set timer for next event after Time.

  To clear the timer without scheduling a timer event, set Time to a
  practically infinite value or mask the timer interrupt by clearing sie.STIE.

  @param[in]  Time                 The time offset to the next scheduled timer interrupt.
**/
VOID
EFIAPI
SbiSetTimer (
  IN  UINT64  Time
  );

///
/// IPI extension
///

/**
  Send IPI to all harts specified in the mask.

  The interrupts are registered as supervisor software interrupts at the
  receiving hart.

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiSendIpi (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase
  );

///
/// Remote fence extension
///

/**
  Instructs remote harts to execute a FENCE.I instruction.

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiRemoteFenceI (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase
  );

/**
  Instructs the remote harts to execute one or more SFENCE.VMA instructions.

  The SFENCE.VMA covers the range of virtual addresses between StartAddr and Size.

  The remote fence function acts as a full tlb flush if * StartAddr and size
  are both 0 * size is equal to 2^XLEN-1

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @param[in]  StartAddr            The first address of the affected range.
  @param[in]  Size                 How many addresses are affected.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_LOAD_ERROR           StartAddr or Size is not valid.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiRemoteSfenceVma (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase,
  IN  UINTN  StartAddr,
  IN  UINTN  Size
  );

/**
  Instructs the remote harts to execute one or more SFENCE.VMA instructions.

  The SFENCE.VMA covers the range of virtual addresses between StartAddr and Size.
  Covers only the given ASID.

  The remote fence function acts as a full tlb flush if * StartAddr and size
  are both 0 * size is equal to 2^XLEN-1

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @param[in]  StartAddr            The first address of the affected range.
  @param[in]  Size                 How many addresses are affected.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_LOAD_ERROR           StartAddr or Size is not valid.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiRemoteSfenceVmaAsid (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase,
  IN  UINTN  StartAddr,
  IN  UINTN  Size,
  IN  UINTN  Asid
  );

/**
  Instructs the remote harts to execute one or more SFENCE.GVMA instructions.

  The SFENCE.GVMA covers the range of virtual addresses between StartAddr and Size.
  Covers only the given VMID.
  This function call is only valid for harts implementing the hypervisor extension.

  The remote fence function acts as a full tlb flush if * StartAddr and size
  are both 0 * size is equal to 2^XLEN-1

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @param[in]  StartAddr            The first address of the affected range.
  @param[in]  Size                 How many addresses are affected.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_LOAD_ERROR           StartAddr or Size is not valid.
  @retval EFI_UNSUPPORTED          SBI does not implement this function or one
                                   of the target harts does not support the
                                   hypervisor extension.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiRemoteHfenceGvmaVmid (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase,
  IN  UINTN  StartAddr,
  IN  UINTN  Size,
  IN  UINTN  Vmid
  );

/**
  Instructs the remote harts to execute one or more SFENCE.GVMA instructions.

  The SFENCE.GVMA covers the range of virtual addresses between StartAddr and Size.
  This function call is only valid for harts implementing the hypervisor extension.

  The remote fence function acts as a full tlb flush if * StartAddr and size
  are both 0 * size is equal to 2^XLEN-1

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @param[in]  StartAddr            The first address of the affected range.
  @param[in]  Size                 How many addresses are affected.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_LOAD_ERROR           StartAddr or Size is not valid.
  @retval EFI_UNSUPPORTED          SBI does not implement this function or one
                                   of the target harts does not support the
                                   hypervisor extension.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiRemoteHfenceGvma (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase,
  IN  UINTN  StartAddr,
  IN  UINTN  Size
  );

/**
  Instructs the remote harts to execute one or more SFENCE.VVMA instructions.

  The SFENCE.GVMA covers the range of virtual addresses between StartAddr and Size.
  Covers only the given ASID.
  This function call is only valid for harts implementing the hypervisor extension.

  The remote fence function acts as a full tlb flush if * StartAddr and size
  are both 0 * size is equal to 2^XLEN-1

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @param[in]  StartAddr            The first address of the affected range.
  @param[in]  Size                 How many addresses are affected.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_LOAD_ERROR           StartAddr or Size is not valid.
  @retval EFI_UNSUPPORTED          SBI does not implement this function or one
                                   of the target harts does not support the
                                   hypervisor extension.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiRemoteHfenceVvmaAsid (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase,
  IN  UINTN  StartAddr,
  IN  UINTN  Size,
  IN  UINTN  Asid
  );

/**
  Instructs the remote harts to execute one or more SFENCE.VVMA instructions.

  The SFENCE.GVMA covers the range of virtual addresses between StartAddr and Size.
  This function call is only valid for harts implementing the hypervisor extension.

  The remote fence function acts as a full tlb flush if * StartAddr and size
  are both 0 * size is equal to 2^XLEN-1

  @param[in]  HartMask             Scalar bit-vector containing hart ids
  @param[in]  HartMaskBase         The starting hartid from which the bit-vector
                                   must be computed. If set to -1, HartMask is
                                   ignored and all harts are considered.
  @param[in]  StartAddr            The first address of the affected range.
  @param[in]  Size                 How many addresses are affected.
  @retval EFI_SUCCESS              IPI was sent to all the targeted harts.
  @retval EFI_LOAD_ERROR           StartAddr or Size is not valid.
  @retval EFI_UNSUPPORTED          SBI does not implement this function or one
                                   of the target harts does not support the
                                   hypervisor extension.
  @retval EFI_INVALID_PARAMETER    Either hart_mask_base or any of the hartid
                                   from hart_mask is not valid i.e. either the
                                   hartid is not enabled by the platform or is
                                   not available to the supervisor.
**/
EFI_STATUS
EFIAPI
SbiRemoteHfenceVvma (
  IN  UINTN  *HartMask,
  IN  UINTN  HartMaskBase,
  IN  UINTN  StartAddr,
  IN  UINTN  Size
  );

///
/// Firmware System Reset (SRST) Extension
///

/**
  Reset the system

  The System Reset Extension provides a function that allow the supervisor
  software to request system-level reboot or shutdown. The term "system" refers
  to the world-view of supervisor software and the underlying SBI
  implementation could be machine mode firmware or hypervisor.

  Valid parameters for ResetType and ResetReason are defined in sbi_ecall_interface.h

  #define SBI_SRST_RESET_TYPE_SHUTDOWN    0x0
  #define SBI_SRST_RESET_TYPE_COLD_REBOOT 0x1
  #define SBI_SRST_RESET_TYPE_WARM_REBOOT 0x2

  #define SBI_SRST_RESET_REASON_NONE      0x0
  #define SBI_SRST_RESET_REASON_SYSFAIL   0x1

  When the call is successful, it will not return.

  @param[in]  ResetType            Typ of reset: Shutdown, cold-, or warm-reset.
  @param[in]  ResetReason          Why the system resets. No reason or system failure.
  @retval EFI_INVALID_PARAMETER    Either ResetType or ResetReason is invalid.
  @retval EFI_UNSUPPORTED          ResetType is valid but not implemented on the platform.
  @retval EFI_DEVICE_ERROR         Unknown error.
**/
EFI_STATUS
EFIAPI
SbiSystemReset (
  IN  UINTN  ResetType,
  IN  UINTN  ResetReason
  );

///
/// Vendor Specific extension space: Extension Ids 0x09000000 through 0x09FFFFFF
///

/**
  Call a function in a vendor defined SBI extension

  ASSERT() if the ExtensionId is not in the designated SBI Vendor Extension
  Space.

  @param[in]  ExtensionId          The SBI vendor extension ID.
  @param[in]  FunctionId           The function ID to call in this extension.
  @param[in]  NumArgs              How many arguments are passed.
  @param[in]  ...                  Actual Arguments to the function.
  @retval EFI_SUCCESS if the SBI function was called and it was successful
  @retval EFI_INVALID_PARAMETER if NumArgs exceeds 6
  @retval others if the called SBI function returns an error
**/
EFI_STATUS
EFIAPI
SbiVendorCall (
  IN  UINTN  ExtensionId,
  IN  UINTN  FunctionId,
  IN  UINTN  NumArgs,
  ...
  );

/**
  Get firmware context of the calling hart.

  @param[out] FirmwareContext      The firmware context pointer.
**/
VOID
EFIAPI
GetFirmwareContext (
  OUT EFI_RISCV_FIRMWARE_CONTEXT  **FirmwareContext
  );

/**
  Set firmware context of the calling hart.

  @param[in] FirmwareContext       The firmware context pointer.
**/
VOID
EFIAPI
SetFirmwareContext (
  IN EFI_RISCV_FIRMWARE_CONTEXT  *FirmwareContext
  );

/**
  Get pointer to OpenSBI Firmware Context

  Get the pointer of firmware context.

  @param    FirmwareContextPtr   Pointer to retrieve pointer to the
                                 Firmware Context.
**/
VOID
EFIAPI
GetFirmwareContextPointer (
  IN OUT EFI_RISCV_FIRMWARE_CONTEXT  **FirmwareContextPtr
  );

/**
  Set pointer to OpenSBI Firmware Context

  Set the pointer of firmware context.

  @param    FirmwareContextPtr   Pointer to Firmware Context.
**/
VOID
EFIAPI
SetFirmwareContextPointer (
  IN EFI_RISCV_FIRMWARE_CONTEXT  *FirmwareContextPtr
  );

#endif
