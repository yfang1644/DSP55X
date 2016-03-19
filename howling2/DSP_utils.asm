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

	.def _DSP_memcpy
; DSP_memcpy(Word16 *dst, Word16 *src, Word16 cnt);
; copy as double word
_DSP_memcpy:
	T0 = T0 >> #1
	T0 -= #1
	CSR = T0
	repeat(CSR)
		dbl(*AR0+) = dbl(*AR1+)
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

	.end
;----------------------------------------------------------------
;End of file

