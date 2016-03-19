;
;-----------------------------------------------------------------------*
;                         pit_shrp.c                                    *
;-----------------------------------------------------------------------*
; Performs Pitch sharpening routine                                     *
;-----------------------------------------------------------------------*
	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.text

	.def	_Pit_shrp
;
;void Pit_shrp(
;     Word16 * x,         /* in/out: impulse response (or algebraic code) */
;     Word16 pit_lag,     /* input : pitch lag                            */
;     Word16 sharp,       /* input : pitch sharpening factor (Q15)        */
;     Word16 L_subfr      /* input : subframe size                        */
;)
;{
;    Word16 i;
;    Word32 L_tmp;
;
;    for (i = pit_lag; i < L_subfr; i++)
;    {
;        tmp = mult_r(x[i - pit_lag], sharp);
;        x[i] = add(x[i], tmp);
;    }
;
;    return;
;}

_Pit_shrp:
	AR1 -= T0						;; AR0 = x
	AR1 -= #1						;; T0 = pit_lag
	if (AR1 < #0)	goto _pit_shrp_end
	BRC0 = AR1						;; T1 = sharp
	bit(ST1, #ST1_FRCT) = #1		;; AR1 = L_subfr
	XAR1 = XAR0						;; AR0 => x[0]
	bit(ST1, #ST1_SATD) = #1

	AC0 = (*AR0+) * T1 || AR1 += T0	;; AR1 => x[pit_lag]
	AC0 = AC0 + ((*AR1) << #16)
	localrepeat {
		*AR1+ = HI(rnd(AC0)) || AC0 = (*AR0+) * T1
		AC0 = AC0 + ((*AR1) << #16)
	}

	bit(ST1, #ST1_FRCT) = #0
_pit_shrp_end:
	bit(ST1, #ST1_SATD) = #0 ||
	return

;OK 2011-12-01
;modified 2012-01-09

