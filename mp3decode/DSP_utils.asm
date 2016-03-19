;****************************************************************
;
; memory copy in assembly languange
;
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.text

	.def _DSP_zero
; DSP_zero(Word16 *buf, Word16 cnt);
; set zero to buf[2*cnt]
_DSP_zero:
	PSHBOTH(AC0) || T0 = T0 >> #1
	T0 -= #1 
	AC0 = #0 || CSR = T0
	repeat(CSR)
		dbl(*AR0+) = AC0
	
	AC0 = POPBOTH()
	return

	.def	_DSP_scale
; DSP_scale(short *dst, long *src, int cnt)
_DSP_scale:
	T0 -= #1
	CSR = T0 || AC0 = dbl(*AR1+)
	repeat(CSR)
		*AR0+ = HI(saturate(rnd(AC0 << #3))) || AC0 = dbl(*AR1+)

	return

	.def _EZDSP5535_waitusec
; _EZDSP5535_waitusec(Word16 cnt);
_EZDSP5535_waitusec:
	T0 -= #1
	BRC0 = T0
	localrepeat {
		repeat(#110)
			nop
	}
	return

	.def	_mad_f_mul
_mad_f_mul:
	T0 = AC0 || AC0 = AC0 * AC1
	AC1 = AC1 * T0 || AC0 = AC0 << #4
    AC0 = AC0 + (AC1 << #-12) || return

	.end
;----------------------------------------------------------------
;End of file

