;-----------------------------------------------------------------------
;                          gpclip.c                                     
;-----------------------------------------------------------------------
;  To avoid unstable synthesis on frame erasure, the gain need to       
;  be limited (gain pitch < 1.0) when the following case occurs:        
;    - a resonance on LPC filter (lp_disp < 60 Hz)                      
;    - a good pitch prediction (lp_gp > 0.95)                           
;-----------------------------------------------------------------------
	.ARMS_off                       ; enable assembler for ARMS=0
	.CPL_on                         ; enable assembler for CPL=1
	.mmregs                         ; enable mem mapped register names

	.text

DIST_ISF_MAX	.set	307                ; 120 Hz (6400Hz=16384)
DIST_ISF_THRES	.set	154                ; 60     (6400Hz=16384)
GAIN_PIT_THRES	.set	14746              ; 0.9 in Q14
GAIN_PIT_MIN	.set	9830               ; 0.6 in Q14
M				.set	16

;
;void Init_gp_clip(
;     Word16 mem[]    /* (o) : memory of gain of pitch clipping algorithm */
;)
;{
;    mem[0] = DIST_ISF_MAX;
;    mem[1] = GAIN_PIT_MIN;
;}
;
	.def	_Init_gp_clip
_Init_gp_clip:
	*AR0+ = #DIST_ISF_MAX
	*AR0+ = #GAIN_PIT_MIN
	return


;Word16 Gp_clip(
;     Word16 mem[]   /* (i/o) : memory of gain of pitch clipping algorithm */
;)
;{
;    Word16 clip;
;
;    clip = 0;
;    if ((sub(mem[0], DIST_ISF_THRES) < 0) && (sub(mem[1], GAIN_PIT_THRES) > 0))
;        clip = 1;
;
;    return (clip);
;}
;
;
	.def	_Gp_clip
_Gp_clip:
	T0 = *AR0+
	T0 -= #DIST_ISF_THRES
	if (T0 >= 0) goto _gp_exit || T0 = #0
	T0 = *AR0+
	T0 -= #GAIN_PIT_THRES
	if (T0 <= 0) goto _gp_exit || T0 = #0
	T0 = #1
_gp_exit:
	return

;void Gp_clip_test_isf(
;     Word16 isf[],      /* (i)   : isf values (in frequency domain)           */
;     Word16 mem[]       /* (i/o) : memory of gain of pitch clipping algorithm */
;)
;{
;    Word16 i, dist, dist_min;
;    Word32 L_tmp;
;
;    dist_min = MAX16;
;
;    for (i = 1; i < M - 1; i++)
;    {
;        dist = sub(isf[i], isf[i - 1]);
;        if (sub(dist, dist_min) < 0)
;        {
;            dist_min = dist;
;        }
;    }
;
;    L_tmp = L_mult(26214, mem[0]);
;    L_tmp += (Word32)13108 * (Word32)dist_min;
;    dist = (Word16)(L_tmp >> 16);
;//    dist = extract_h(L_mac(L_mult(26214, mem[0]), 6554, dist_min));
;
;    if (sub(dist, DIST_ISF_MAX) > 0)
;    {
;        dist = DIST_ISF_MAX;
;    }
;    mem[0] = dist;
;
;    return;
;}
;
	.def	_Gp_clip_test_isf
_Gp_clip_test_isf:
	PUSH(T0, T1)
	PUSH(T2, T3)
	PSHBOTH(AC0)

	T0 = *AR0+
	T2 = #32767
	T1 = *AR0
	BRC0 = #(M - 3)
	localrepeat {
		T1 = T1 - T0 || T0 = *AR0+
		T2 = MIN(T1, T2) || T1 = *AR0
	}

	T1 = #26214						; T1 = 0.8
	AC0 = (*AR1+) * T1
	T0 = #6554						; T0 = 0.2
	AC0 = AC0 + (*(#20h) * T2)

	*(#20h) = HI(AC0 << #1)
	T1 = #DIST_ISF_MAX
	T0 = MIN(T1, T0)
	*AR1 = T0

	AC0 = POPBOTH()
	T2, T3 = POP()
	T0, T1 = POP()
	return

;
;void Gp_clip_test_gain_pit(
;     Word16 gain_pit,   /* (i) Q14 : gain of quantized pitch               */
;     Word16 mem[]   /* (i/o)   : memory of gain of pitch clipping algorithm */
;)
;{
;    Word16 gain;
;    Word32 L_tmp;
;
;    L_tmp = L_mult(29491, mem[1]);
;    L_tmp = L_mac(L_tmp, 3277, gain_pit);
;    gain = (Word16)(L_tmp >> 16);
;
;    if (sub(gain, GAIN_PIT_MIN) < 0)
;    {
;        gain = GAIN_PIT_MIN;
;    }
;    mem[1] = gain;
;
;    return;
;}

	.def	_Gp_clip_test_gain_pit
_Gp_clip_test_gain_pit:
	PSHBOTH(AC0)
	PUSH(T0, T1)
	T1 = #29491						; T1 = 0.9
	AR0 += #1
	AC0 = (*AR0) << #16
	AC0 = AC0 * T1 || T1 = #3277	; T1 = 0.1
	AC0 = AC0 + (*(#20h) * T1)

	*(#20h) = HI(AC0 << #1)
	T1 = #GAIN_PIT_MIN
	T0 = MAX(T1, T0)
	*AR0 = T0
	T0, T1 = POP()
	AC0 = POPBOTH()
	return

; 2011-12-03
