;****************************************************************
; Function:    DSP_calc
; Processor:   C55xx
;
;
; Usage: void DSP_calc(Int32 var1, Int32 var2, Int16 var3....)
;     First Int16 as T0, second as T1,...(third may be AR0?)
;     First Int32 as AC0, second as AC1, third as AC2
;
;     return Int32 in AC0
;     return Int16 in T0
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.asg	AC0, var1				; pointer for load
	.asg	AC1, var2				; pointer for filter coefficients
	.asg	T0,  var3				;
	.asg	AR3, var3

	.text

	.def _satu
_satu:

	PUSH (@ST3_L) || mmap()
	bit(ST1, #ST1_SATD) = #1
	T0 = LO(AC0)
	@ST3_L = POP() || mmap()

	return


	.def _extract_h
_extract_h:

	T0 = HI(AC0)
	return


	.def	_add
_add:

	PUSH (@ST3_L) || mmap()

	bit(ST3, #ST3_SATA) = #1
	T0 += T1
	@ST3_L = POP() || mmap()
	return

	.def	_sub
_sub:

	PUSH (@ST3_L) || mmap()

	bit(ST3, #ST3_SATA) = #1
	T0 -= T1
	@ST3_L = POP() || mmap()
	return

	.def	_negate
_negate:
	PUSH (@ST3_L) || mmap()
	bit(ST3, #ST3_SATA) = #1
	T0 = -T0
	@ST3_L = POP() || mmap()
	return

	.def	_round16
_round16:
	PUSH (@ST2_L) || mmap()
	bit(ST2, #ST2_RDM) = #1
	AC0 = rnd(AC0)
	T0 = HI(AC0)
	@ST2_L = POP() || mmap()
	return


	.def	_abs_s
_abs_s:
	PUSH (@ST3_L) || mmap()

	bit(ST3, #ST3_SATA) = #1
	T0 = |T0|
	@ST3_L = POP() || mmap()
	return

	.def	_shr
_shr:
	PSHBOTH(AC0) || LO(AC0) = T0
	T1 = -T1
	AC0 = AC0 << T1
	T0 = LO(AC0) || AC0 = POPBOTH()

	return

	.def	_shl
_shl:
	PSHBOTH(AC0) || LO(AC0) = T0
	AC0 = AC0 << T1
	T0 = LO(AC0) || AC0 = POPBOTH()
	return

	.def	_L_add
_L_add:
	AC0 += AC1
	AC0 = saturate(AC0)||
	return

	.def	_L_sub
_L_sub:
	AC0 -= AC1
	AC0 = saturate(AC0)||
	return


	.def	_L_mult0
_L_mult0:
	HI(AC0) = T0
	AC0 = AC0 * T1
	return


	.def	_L_mult
_L_mult:
	PUSH (@ST1_L) || mmap()

	bit(ST1, #ST1_SATD) = #1 ||
	HI(AC0) = T0
	AC0 = AC0 * T1
	AC0 = AC0 << #1
	@ST1_L = POP() || mmap()
	return

	.def	_L_mac
_L_mac:
	PUSH (@ST1_L) || mmap()
	PSHBOTH(AC1)
	bit(ST1, #ST1_SATD) = #1 ||
	HI(AC1) = T0
	AC1 = AC1 * T1
	AC0 = AC0 + (AC1<<1) || AC1 = POPBOTH()

	@ST1_L = POP() || mmap()
	return

	.def	_L_msu
_L_msu:
	PUSH (@ST1_L) || mmap()
	PSHBOTH(AC1)
	bit(ST1, #ST1_SATD) = #1 ||
	HI(AC1) = T0
	AC1 = AC1 * T1
	AC0 = AC0 - (AC1<<1) || AC1 = POPBOTH()

	@ST1_L = POP() || mmap()
	return

	.def	_L_shl
_L_shl:

	PUSH (@ST1_L) || mmap()
	bit(ST1, #ST1_SATD) = #1
	AC0 = AC0 << T0
	@ST1_L = POP() || mmap()
	return

	.def	_L_shr
_L_shr:
	PUSH (@ST1_L) || mmap()
	bit(ST1, #ST1_SATD) = #1
	T0 = -T0
	AC0 = AC0 << T0
	@ST1_L = POP() || mmap()
	return

	.def	_shr_rr
_shr_rr:
	PUSH (@ST2_L) || mmap()
	bit(ST2, #ST2_RDM) = #1
	T1 = -T1
	PSHBOTH(AC0) || LO(AC0) = T0
	AC0 = AC0 << T1
	T0 = LO(AC0) || AC0 = POPBOTH()
	@ST2_L = POP() || mmap()
	return


;----------------------------------------------------------------
;End of file

