;-------------------------------------------------------------------*
;                         preemph.c                                 *
;-------------------------------------------------------------------*
; Preemphasis: filtering through 1 - g z^-1                         *
;                                                                   *
; Preemph2 --> signal is multiplied by 2.                           *
;-------------------------------------------------------------------*/

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.noremark	5673, 5584
	.text
;
;void Preemph(
;     Word16 x[],    /* (i/o)   : input signal overwritten by the output */
;     Word16 mu,     /* (i) Q15 : preemphasis coefficient                */
;     Word16 lg      /* (i)     : lenght of filtering                    */
;)
;{
;    Word16 i;
;    Word32 L_tmp;
;
;    for (i = (lg - 1); i > 0; i--)
;    {
;        L_tmp = (Word32)x[i] << 16;
;        L_tmp = L_msu(L_tmp, x[i - 1], mu);
;        x[i] = round16(L_tmp);
;    }
;
;    return;
;}
	.def	_Preemph
_Preemph:
	T1 -= #1
	AR0 = AR0 + T1
	T1 -= #1
	BRC0 = T1

	XAR1 = XAR0
	AR0 -= #1
	bit(ST1, #ST1_FRCT) = #1
	AC0 = (*AR1-) << #16
	localrepeat {
		AC0 = (*AR1+) << #16 || AC1 = AC0 - ((*AR0-) * T0)
		*AR1 = HI(saturate(rnd(AC1))) || AR1 -= #2
	}

	bit(ST1, #ST1_FRCT) = #0 ||
	return


;void Preemph2(
;     Word16 x[],    /* (i/o)   : input signal overwritten by the output */
;     Word16 mu,     /* (i) Q15 : preemphasis coefficient                */
;     Word16 lg      /* (i)     : lenght of filtering                    */
;)
;{
;    Word16 i, temp, lg_1;
;    Word32 L_tmp;
;
;    lg_1 = lg - 1;
;    temp = x[lg_1];
;
;    for (i = lg_1; i > 0; i--)
;    {
;        L_tmp = (Word32)x[i] << 16;
;        L_tmp = L_msu(L_tmp, x[i - 1], mu);
;        L_tmp = L_shl(L_tmp, 1);
;        x[i] = round16(L_tmp);
;    }
;
;    x[0] <<= 1;
;
;    return;
;}

	.def	_Preemph2
_Preemph2:
	T1 -= #1
	AR0 = AR0 + T1
	T1 -= #1
	BRC0 = T1

	XAR1 = XAR0
	AR0 -= #1
	bit(ST1, #ST1_FRCT) = #1
	AC0 = (*AR1-) << #16

	localrepeat {
		AC0 = (*AR1+) << #16 || AC1 = AC0 - ((*AR0-) * T0)
		*AR1 = HI(saturate(rnd(AC1 << #1))) || AR1 -= #2
	}

	AR1 += #1
	AC0 = (*AR1) << #16
	*AR1 = HI(rnd(AC0 << #1))

	bit(ST1, #ST1_FRCT) = #0 ||
	return

;OK 2011-12-01
;modified 2012-01-04

