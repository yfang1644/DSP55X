;******************************************************************************
;* udiv.asm  v4.3.8
;* Copyright (c) 1997-2010 Texas Instruments Incorporated
;******************************************************************************
	.include "c55xasm.i"
	.mmregs
	.cpl_on
	.arms_on
	.model  call=internal ; This function is recognized by name 
			      ; in the compiler.

******************************************************************************
* 16-BIT UNSIGNED DIVIDE
*
* arg1 	 : dividend - T0  (U)
* arg2 	 : divisor  - T1  (V)
* return : quotient - T0  (Q == U / V)
*
*
* WARNING!  Changes to register defs in this function must be reflected in
* the constraint class DIVU_SET in ld3.md
******************************************************************************
	.global	I$$UDIV
	.global __divu

	.if __TMS320C55X_PLUS_BYTE__
FRAME_SIZE	.set	2
	.else
FRAME_SIZE	.set	1
	.endif

I$$UDIV:	.asmfunc
__divu:
******************************************************************************
* PREPARATION
******************************************************************************
	SP = SP - #FRAME_SIZE
	bit(ST1, #ST1_SXMD) = #0 	; turn off SXM mode
	AC0 = T0			; Move Q to AC0
||	*SP(#0) = T1			; Store Divisor in memory

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
	repeat(#15)             	; repeat 16 times
	subc(*SP(#(0)), AC0, AC0)       ; divide step

******************************************************************************
* QUOTIENT IS IN LO(AC0), REMAINDER IS IN HI(AC0)
******************************************************************************
	bit(ST1, #ST1_SXMD) = #1 	; turn on SXM mode
	T0 = AC0 			; discard remainder

	SP = SP + #FRAME_SIZE

	return                  	; return
	.endasmfunc

******************************************************************************
* 16-BIT UNSIGNED MODULUS
*
* arg1 	 : dividend  - T0  (U)
* arg2 	 : divisor   - T1  (V)
* return : remainder - T0  (Q == U % V)
*
*
* WARNING!  Changes to register defs in this function must be reflected in
* the constraint class REMU_SET in ld3.md
******************************************************************************
	.global	I$$UMOD
	.global __remu

I$$UMOD:	.asmfunc
__remu:
******************************************************************************
* PREPARATION
******************************************************************************
	SP = SP - #FRAME_SIZE
	bit(ST1, #ST1_SXMD) = #0 	; turn off SXM mode
	AC0 = T0			; Move Q to AC0
||	*SP(#0) = T1			; Store Divisor in memory

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
	repeat(#15)             	; repeat 16 times
	subc(*SP(#(0)), AC0, AC0)       ; divide step

******************************************************************************
* QUOTIENT IS IN LO(AC0), REMAINDER IS IN HI(AC0)
******************************************************************************
	bit(ST1, #ST1_SXMD) = #1 	; turn on SXM mode
	T0 = HI(AC0)		      	; extract remainder from AC0_H
	SP = SP + #FRAME_SIZE

	return                 		; return
	.endasmfunc
