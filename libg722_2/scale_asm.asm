;*********************************************************************
; -------------------------------------------------------------------*
;  Scale signal to get maximum of dynamic.                           *
; -------------------------------------------------------------------*
;
;void Scale_sig(
;     Word16 x[],       /* (i/o) : signal to scale               */
;     Word16 lg,        /* (i)   : size of x[]                   */
;     Word16 exp        /* (i)   : exponent: x = round(x << exp) */
;)
;{
;    for (i = 0; i < lg; i++)
;    {
;        L_tmp = (Word32)x[i] << 16;
;        L_tmp = L_shl(L_tmp, exp);   /* saturation can occur here */
;        x[i] = round16(L_tmp);
;    }
;}
;
;*********************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names
	.noremark	5673
	.text

	.def	_Scale_sig
_Scale_sig:
	PSHBOTH(XAR1)
	T0 -= #1
	XAR1 = XAR0
	CSR = T0 || PSHBOTH(AC0)

	AC0 = (*AR0+) << #16
	repeat (CSR)
		AC0 = (*AR0+) << #16 || *AR1+ = HI(saturate(rnd(AC0 << T1)))

	AC0 = POPBOTH()
	XAR1 = POPBOTH()

	bit(ST1, #ST1_SATD) = #0 ||
	return

