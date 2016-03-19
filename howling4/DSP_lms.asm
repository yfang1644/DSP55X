;****************************************************************
; Function:    DSP_lms
; Processor:   C55xx
;
; Usage: short DSP_lms(
;       short *x,   // Pointer to input delay buffer(reference channel)
;       long *flt,  // Pointer to the coefficient array
;       int mu,     // adaptive rate, 2^mu
;       int error,
;       int nh)     // Number of output samples
;{
;    int i;
;    int sum = 0;
;    int temp1, temp2;
;    int s0, s1, s2;
;    int e_mu;
;
;    e_mu = -32;                    // mu = 2^(-32), max. convergency
;    s1 = 5;                        // Q. bit integer, s1=5 means Q4.27
;    s0 = s1 - (32 + e_mu);
;    for (i = nh; i > 0; i--)
;    {
;        temp1 = (int)x[i] * error;
;        h[i] += temp1 >> (s0);     // h[i] += x[i] * ae;
;        x[i] = x[i - 1];
;        temp2 = (int)x[i] * (h[i] >> 16);    // sum += x[i] * h[i];
;        sum += (temp2 >> 8);                 // shift to avoid overflow
;    }
;    sum = sum >> (16 - s1 - 8);
;    return (short)sum;
;}
;
; mu must be power of a negative integer(simplify)
; mu = -32 means mu = 2^(-32)
;
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names
	.noremark	5584, 5673			; mant(), exp()

	.asg	AR0, x					; point to x(reference array)
	.asg	AR1, flt				; pointer to adaptive filter coefficents
	.def	_DSP_lms
	; T0 = mu
	; T1 = error
	; AR2 = cnt
	.text

_DSP_lms:
	AC0 = XAR0				|| PSHBOTH(AC0)
	AC0 = AC0 + (*(#12h) << #0)						; *(#12h) = AR2
	XAR0 = AC0				|| PSHBOTH(AC1)
	AR2 -= #1
	BRC0 = AR2				|| PSHBOTH(AC2)
	AC0 = #0				|| bit(ST1, #ST1_SATD) = #1
	AC1 = dbl(*flt)			|| bit(ST1, #ST1_FRCT) = #1
	bit(ST1, #ST1_M40) = #1	|| blockrepeat {
		AC2 = (*x-) * T1							; AC1 = h[i]
		AC1 += (AC2 << T0)	|| delay(*x)			; x[i] = x[i-1]
		AC2 = AC1 * (*x)	|| dbl(*flt+) = AC1		; save h[i]
		AC0 += AC2 << #-6	|| AC1 = dbl(*flt)
	}
	*(#20h) = HI(rnd(AC0 << #12))					; Q6.25
	AC2 = POPBOTH()			|| bit(ST1, #ST1_FRCT) = #0
	AC1 = POPBOTH()			|| bit(ST1, #ST1_SATD) = #0
	AC0 = POPBOTH()			|| bit(ST1, #ST1_SATD) = #0
	return
	.end
;----------------------------------------------------------------
;End of file

