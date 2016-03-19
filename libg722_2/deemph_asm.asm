	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.noremark	5673, 5584
	.text

;-------------------------------------------------------------------*
;                          deemph.c                                 *
;-------------------------------------------------------------------*
; Deemphasis: filtering through 1/(1-mu z^-1)                       *
;                                                                   *
; Deemph2   --> signal is divided by 2.                             *
; Deemph_32 --> for 32 bits signal.                                 *
;-------------------------------------------------------------------*/
;
;void Deemph(
;     Word16 x[],       /* (i/o)   : input signal overwritten by the output */
;     Word16 mu,        /* (i) Q15 : deemphasis factor                      */
;     Word16 L,         /* (i)     : vector size                            */
;     Word16 * mem      /* (i/o)   : memory (y[-1])                         */
;)
;{
;    Word16 i;
;    Word32 L_tmp;
;
;    L_tmp = (Word32)x[0] << 16;
;    L_tmp = L_mac(L_tmp, *mem, mu);
;    x[0] = round16(L_tmp);
;
;    for (i = 1; i < L; i++)
;    {
;        L_tmp = (Word32)x[i] << 16;
;        L_tmp = L_mac(L_tmp, x[i - 1], mu);
;        x[i] = round16(L_tmp);
;    }
;
;    *mem = x[L - 1];
;
;    return;
;}
;
	.def	_Deemph
_Deemph:
	PUSH (@ST1_L) || mmap()
	T1 -= #2
	BRC0 = T1
	bit(ST1, #ST1_FRCT) = #1 || PSHBOTH(AC0)
	bit(ST1, #ST1_SATD) = #1 || AC0 = (*AR1) * T0
	AC0 = AC0 + ((*AR0) << #16)
	*AR0 = HI(rnd(AC0))
	localrepeat {
		AC0 = (*AR0+) * T0
		AC0 = AC0 + ((*AR0) << #16)
		*AR0 = HI(rnd(AC0))
	}

	*AR1 = *AR0
	AC0 = POPBOTH()
	@ST1_L = POP() || mmap()
	return

;void Deemph2(
;     Word16 x[],         /* (i/o)   : input signal overwritten by the output */
;     Word16 mu,          /* (i) Q15 : deemphasis factor                      */
;     Word16 L,           /* (i)     : vector size                            */
;     Word16 * mem        /* (i/o)   : memory (y[-1])                         */
;)
;{
;    Word16 i;
;    Word32 L_tmp;
;
;    /* saturation can occur in L_mac() */
;
;    L_tmp = L_mult(x[0], 16384);
;    L_tmp = L_mac(L_tmp, *mem, mu);
;    x[0] = round16(L_tmp);
;
;    for (i = 1; i < L; i++)
;    {
;        L_tmp = L_mult(x[i], 16384);
;        L_tmp = L_mac(L_tmp, x[i - 1], mu);
;        x[i] = round16(L_tmp);
;    }
;
;    *mem = x[L - 1];
;
;    return;
;}
;
	.def	_Deemph2
_Deemph2:
	PUSH (@ST1_L) || mmap()
	T1 -= #2
	BRC0 = T1
	bit(ST1, #ST1_FRCT) = #1 || PSHBOTH(AC0)
	bit(ST1, #ST1_SATD) = #1 || AC0 = (*AR1) * T0
	AC0 = AC0 + ((*AR0) << #15)
	*AR0 = HI(rnd(AC0))
	localrepeat {
		AC0 = (*AR0+) * T0
		AC0 = AC0 + ((*AR0) << #15)
		*AR0 = HI(rnd(AC0))
	}

	*AR1 = *AR0

	AC0 = POPBOTH()
	@ST1_L = POP() || mmap()
	return

;void Deemph_32(
;     Word16 x_hi[],      /* (i)     : input signal (bit31..16) */
;     Word16 x_lo[],      /* (i)     : input signal (bit15..4)  */
;     Word16 y[],         /* (o)     : output signal (x16)      */
;     Word16 mu,          /* (i) Q15 : deemphasis factor        */
;     Word16 L,           /* (i)     : vector size              */
;     Word16 * mem        /* (i/o)   : memory (y[-1])           */
;)
;{
;    Word16 i, fac;
;    Word32 L_tmp;
;
;    /* L_tmp = hi<<16 + lo<<4 */
;
;    L_tmp = (Word32)x_hi[0] << 12;
;    L_tmp = L_tmp + (Word32)x_lo[0];
;    L_tmp = L_shl(L_tmp, 8);
;    L_tmp = L_mac(L_tmp, *mem, mu);
;    y[0] = round16(L_tmp);
;
;    for (i = 1; i < L; i++)
;    {
;        L_tmp = (Word32)x_hi[i] << 12;
;        L_tmp = L_tmp + (Word32)x_lo[i];
;        L_tmp = L_shl(L_tmp, 8);
;        L_tmp = L_mac(L_tmp, y[i - 1], mu);
;        y[i] = round16(L_tmp);
;    }
;
;    *mem = y[L - 1];
;
;    return;
;}

	.def	_Deemph_32
_Deemph_32:
	PUSH (@ST1_L) || mmap()
	T1 -= #2
	BRC0 = T1

	bit(ST1, #ST1_FRCT) = #1 || PSHBOTH(AC1)
	bit(ST1, #ST1_SATD) = #1 || PSHBOTH(AC0)
	AC1 = (*AR3) * T0
	
	AC0 = (*AR0+) << #16
	localrepeat {
		AC0 = AC0 + ((*AR1+) << #4)
		AC1 = AC1 + (AC0 << #4)
		*AR2 = HI(rnd(AC1)) || AC0 = (*AR0+) << #16
		AC1 = (*AR2+) * T0
	}

	AC0 = AC0 + ((*AR1+) << #4)
	AC1 = AC1 + (AC0 << #4) || AC0 = POPBOTH()
	*AR2 = HI(rnd(AC1))
	*AR3 = *AR2

	AC1 = POPBOTH()
	@ST1_L = POP() || mmap()
	return

;modified 2012-01-09

