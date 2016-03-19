;****************************************************************
;___________________________________________________________________________
;                                                                           |
; Basic arithmetic operators.                                               |
;                                                                           |
; $Id $                                                                     |
;___________________________________________________________________________|
;
; Function:    DSP_calc
; Processor:   C55xx
;
; Usage: void DSP_calc(Word32 var1, Word32 var2, Word16 var3....)
;     First Word16 as T0, second as T1,...(third may be AR0?)
;     First Word32 as AC0, second as AC1, third as AC2
;
;     return Word32 in AC0
;     return Word16 in T0
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.noremark	5673

	.text

;	.def	_norm_l
;___________________________________________________________________________
;                                                                           |
;   Function Name : norm_l                                                  |
;                                                                           |
;   Purpose :                                                               |
;                                                                           |
;   Produces the number of left shifts needed to normalize the 32 bit varia-|
;   ble L_var1 for positive values on the interval with minimum of          |
;   1073741824 and maximum of 2147483647, and for negative values on the in-|
;   terval with minimum of -2147483648 and maximum of -1073741824; in order |
;   to normalize the result, the following operation must be done :         |
;                   norm_L_var1 = L_shl(L_var1,norm_l(L_var1)).             |
;                                                                           |
;   Complexity weight : 30                                                  |
;                                                                           |
;   Inputs :                                                                |
;                                                                           |
;    L_var1                                                                 |
;             32 bit long signed integer (Word32) whose value falls in the  |
;             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
;                                                                           |
;   Outputs :                                                               |
;                                                                           |
;    none                                                                   |
;                                                                           |
;   Return Value :                                                          |
;                                                                           |
;    var_out                                                                |
;             16 bit short signed integer (Word16) whose value falls in the |
;             range : 0x0000 0000 <= var_out <= 0x0000 001f.                |
;___________________________________________________________________________|
;
_norm_l:
	AC0 = AC0 | AC0
	AC0 = mant(AC0), T0 = -exp(AC0)
	T0 = -T0
	*AR0 = T0 || return

	.def	_arrMax
;	max = arrMax(array, length)
_arrMax:
	T0 -= #1
	BRC0 = T0
	bit(ST3, #ST3_SATA) = #1 || T1 = *AR0+
	T0 = #0 || T1 = |T1|
	localrepeat {
		T1 = *AR0+ || T0 = max(T1, T0)
		T1 = |T1|
	}

	bit(ST3, #ST3_SATA) = #0 ||
	return

	.def	_arrMpy
_arrMpy:
	T1 -= #1
	CSR = T1
	bit(ST1, #ST1_FRCT) = #1 ||
	PSHBOTH(XAR1)
	XAR1 = XAR0
	AC0 = (*AR0+) * T0
	repeat (CSR)
		*AR1+ = HI(AC0) || AC0 = (*AR0+) * T0

	XAR1 = POPBOTH()
	bit(ST1, #ST1_FRCT) = #0 ||
	return


	.def	_median5
_median5:
	AR1 = *AR0+
	AR2 = *AR0+ ||  T0 = AR2
	AR3 = *AR0+ || AR2 = max(AR1, AR2)
	AR4 = *AR0+ || AR1 = min(T0, AR1)			;; 1<2
	T0  =  AR4
	TC1 = AR1 > AR3 || nop
	if (TC1) execute(AD_unit) || swap(AR1, AR3)	;; 1<(2,3)
	AR0 = *AR0  || AR4 = max(AR1, AR4)
	AR1 = min(T0, AR1)							;; 1<(2,3,4)
	AR0 = max(AR1, AR0)							;; 1<(2,3,4,5),remove 1

	T0  = AR3
	AR3 = max(AR2, AR3)							;; 
	AR2 = min(T0, AR2)							;; 2<3
	T0  = AR4
	AR4 = max(AR2, AR4)
	AR2 = min(T0, AR2)							;; 2<(3,4)
	TC1 = AR2 > AR0 || nop
	if (TC1) execute(AD_unit) || swap(AR2, AR0)	;; 2<(3,4,5)

	T0  = AR0
	T0  = min(AR3, T0)
	T0  = min(AR4, T0)

	return

	.def	_Dot_product
;Word32 Dot_product(          /* (o) Q31:                                   */
;     Word16 x[],             /* (i) 12bits: x vector                       */
;     Word16 y[],             /* (i) 12bits: y vector                       */
;     Word16 lg               /* (i)    : vector length                     */
;)
;{
;    Word16 i;
;    Word32 L_sum;

;    L_sum = 1L;
;    for (i = 0; i < lg; i++)
;        L_sum = L_mac(L_sum, x[i], y[i]);
;
;    return (L_sum);
;}

_Dot_product:
	PUSH (@ST1_L) || mmap()
	T0 -= #1
	CSR = T0 || AC0 = #0
	bit(ST1, #ST1_FRCT) = #1
	bit(ST1, #ST1_SATD) = #1 ||
	repeat (CSR)
		AC0 = AC0 + ((*AR0+) * (*AR1+))

	@ST1_L = POP() || mmap()
	return


	.def	_Dot_product12
;___________________________________________________________________________
;                                                                           |
;   Function Name : Dot_product12()                                         |
;                                                                           |
;       Compute scalar product of <x[],y[]> using accumulator.              |
;                                                                           |
;       The result is normalized (in Q31) with exponent (0..30).            |
;---------------------------------------------------------------------------|
;  Algorithm:                                                               |
;                                                                           |
;       dot_product = sum(x[i]*y[i])     i=0..N-1                           |
;___________________________________________________________________________|
;
;
;Word32 Dot_product12(        /* (o) Q31: normalized result (1 < val <= -1) */
;     Word16 x[],             /* (i) 12bits: x vector                       */
;     Word16 y[],             /* (i) 12bits: y vector                       */
;     Word16 lg,              /* (i)    : vector length                     */
;     Word16 * exp            /* (o)    : exponent of result (0..+30)       */
;)
;{
;    Word32 L_sum;
;
;    L_sum = Dot_product(x, y, lg);
;
;	*exp = exp_mant0(&L_sum);
;    return (L_sum);
;}

_Dot_product12:
	PUSH (@ST1_L) || mmap()
	T0 -= #1
	CSR = T0 || AC0 = #1
	bit(ST1, #ST1_FRCT) = #1 || PUSH(T0)
	bit(ST1, #ST1_SATD) = #1 ||
	repeat (CSR)
		AC0 = AC0 + ((*AR0+) * (*AR1+))

	AC0 = mant(AC0), T0 = -exp(AC0)
	*AR2 = T0 || T0 = POP()
	@ST1_L = POP() || mmap()
	return

;----------------------------------------------------------------
;End of file

