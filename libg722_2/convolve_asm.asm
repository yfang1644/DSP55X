;------------------------------------------------------------------------*
;                         convolve.c                                     *
;------------------------------------------------------------------------*
; Perform the convolution between two vectors x[] and h[] and            *
; write the result in the vector y[].                                    *
; All vectors are of length L.                                           *
;------------------------------------------------------------------------*

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.noremark	5673, 5584
	.text


;void Convolve(
;     Word16 x[],            /* (i)        : input vector                 */
;     Word16 h[],            /* (i) Q15    : impulse response             */
;     Word16 y[],            /* (o) 12 bits: output vector                */
;     Word16 L               /* (i)        : vector size                  */
;)
;{
;    Word16 i, n;
;    Word32 L_sum;
;
;    for (n = 0; n < L; n++)
;    {
;        L_sum = 0L;
;        for (i = 0; i <= n; i++)
;            L_sum = L_mac(L_sum, x[i], h[n - i]);
;
;        y[n] = round16(L_sum);
;    }
;
;    return;
;}

	.def	_Convolve
_Convolve:
	PUSH (@ST1_L) || mmap()
	T0 -= #1
	bit(ST1, #ST1_FRCT) = #1
	bit(ST1, #ST1_SATD) = #1
	BRC0 = T0
	T0 = #0
	localrepeat {
		PUSH(AR0, AR1) || AC0 = #0
		CSR = T0
		repeat(CSR)
			AC0 = AC0 + ((*AR0+) * (*AR1-))

		AR0, AR1 = POP()
		*AR2+ = HI(rnd(AC0)) || T0 += #1
		AR1 +=  #1
	}

	@ST1_L = POP() || mmap()
	return

;modified 2012-01-11

