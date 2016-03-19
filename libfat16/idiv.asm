******************************************************************************
* idiv.asm  v4.3.8
* Copyright (c) 1997-2010 Texas Instruments Incorporated
******************************************************************************
	.mmregs
	.cpl_on
	.arms_on
	.sect	".text"
	.model  call=internal ; This function is recognized by name 
			      ; in the compiler.
	.noremark 5108        ; The FRAME instructions used in this file are
                              ; safe wrt the P2/P3 pipeline difference

******************************************************************************
* 16-BIT SIGNED DIVIDE
*
* arg1 	 : dividend - T0  (U)
* arg2 	 : divisor  - T1  (V)
* return : quotient - T0  (Q == U / V)
*
*
* WARNING!  Changes to register defs in this function must be reflected in
* the constraint class DIVI_SET in ld3.md
******************************************************************************
	.global	I$$DIV
	.global __divi

	.if __TMS320C55X_PLUS_BYTE__
FRAME_SIZE	.set	2
	.else
FRAME_SIZE	.set	1
	.endif

I$$DIV:	.asmfunc
__divi:

******************************************************************************
* DETERMINE SIGN OF RESULT, TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
	SP = SP - #FRAME_SIZE		; allocate space

	AR4 = T1	         	; save D (for sign)
||	AC1 = |T1|             		; take absolute value of D

	AC0 = |T0|             		; take absolute value of Q
||	*SP(#0) = AC1         		; store D

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
	bit(ST1, #ST1_SXMD) = #0 	; turn off SXM mode
||	repeat(#15)             	; repeat 16 times
	subc(*SP(#0), AC0, AC0)       	; divide step

	bit(ST1, #ST1_SXMD) = #1 	; turn on SXM mode

******************************************************************************
* QUOTIENT IS IN AC0_L, REMAINDER IS IN AC0_H
******************************************************************************
	AR4 = AR4 ^ T0			; determine sign of Q
        T0 = AC0			; discard remainder
	if (AR4 < #0) execute(D_Unit)
||	T0 = -T0              		; negate Q

	SP = SP + #FRAME_SIZE	 	; deallocate frame
	return
	.endasmfunc

******************************************************************************
* 16-BIT SIGNED MODULUS
*
* arg1 	 : dividend  - T0  (U)
* arg2 	 : divisor   - T1  (V)
* return : remainder - T0  (Q == U % V)
*
*
* WARNING!  Changes to register defs in this function must be reflected in
* the constraint class REMI_SET in ld3.md
******************************************************************************
	.global	I$$MOD
	.global __remi

I$$MOD:	.asmfunc
__remi:
******************************************************************************
* DETERMINE SIGN OF RESULT, TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
	SP = SP - #FRAME_SIZE		; allocate frame
||	AC1 = |T1|

	AC0 = |T0|
||	*SP(#0) = AC1                   ; save off

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
	bit(ST1, #ST1_SXMD) = #0 	; turn off SXM mode
||	repeat(#15)             	; repeat 16 times
	subc(*SP(#0), AC0, AC0)	        ; divide step

	bit(ST1, #ST1_SXMD) = #1 	; turn on SXM mode

******************************************************************************
* QUOTIENT IS IN ACO_L, REMAINDER IS IN AC0_H
******************************************************************************
	T1 = T0
	T0 = HI(AC0)		      	; extract remainder from AC0_H
	if (T1 >= #0) goto RET2
	T0 = -T0              		; negate Q
RET2:
	SP = SP + #FRAME_SIZE		; deallocate frame

	return                  	; return
	.endasmfunc
	.end


