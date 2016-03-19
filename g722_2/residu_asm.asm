;-----------------------------------------------------------------------*
;                         residu.c                                      *
;-----------------------------------------------------------------------*
; Compute the LPC residual by filtering the input speech through A(z)   *
;-----------------------------------------------------------------------*
;
;void Residu(
;     Word16 a[],      /* (i) Q12 : prediction coefficients             */
;     Word16 m,        /* (i)     : order of LP filter                  */
;     Word16 x[],      /* (i)     : speech (values x[-m..-1] are needed */
;     Word16 y[],      /* (o) x2  : residual signal                     */
;     Word16 lg        /* (i)     : size of filtering                   */
;)
;{
;    Word16 i, j;
;    Word32 s;
;
;    for (i = 0; i < lg; i++)
;    {
;        s = 0L;
;        for (j = 0; j <= m; j++)
;        {
;            s = L_mac(s, a[j], x[i - j]);
;        }
;        s = L_shl(s, 3 + 1);
;        y[i] = round16(s);
;    }
;
;    return;
;}

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.text

	.def _Residu
_Residu:
	T1 -= #1
	bit(ST1, #ST1_FRCT) = #1
	bit(ST1, #ST1_SATD) = #1
	BRC0 = T1
	CSR = T0 || localrepeat {
		PUSH(AR0, AR1) || AC0 = #0
		repeat(CSR)
			AC0 = AC0 + ((*AR0+) * (*AR1-))

		AR0, AR1 = POP()
		*AR2+ = HI(rnd(AC0 << #4)) || 
		AR1 +=  #1
	}

	bit(ST1, #ST1_FRCT) = #0
	bit(ST1, #ST1_SATD) = #0 ||
	return










