;****************************************************************
;
;
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.text

	.def _DSP_word2byte
; void DSP_word2byte(Uint8 *dst, const Int16 *src, int cnt)
; FOR BIG-ENDIAN
_DSP_word2byte:
	PSHBOTH(AC0)
	T0 -= #1
	BRC0 = T0 || AC0 = uns(*AR1+)
	localrepeat{
		*AR0+ = HI(AC0 << #8) || AC0 = AC0 & #0xff
		*AR0+ = LO(AC0 << #0) || AC0 = uns(*AR1+)
	}
	AC0 = POPBOTH()
	return

	.def _DSP_byte2word
; void byte2word(Uint16 *dst, const Uint8 *src, int cnt)
; FOR BIG-ENDIAN
_DSP_byte2word:
	PSHBOTH(AC0)
	T0 -= #1
	BRC0 = T0 || AC0 = *AR1+
	localrepeat{
		AC0 = AC0 <<< #8 || T0 = uns(low_byte(*AR1+))
		AC0 = AC0 | T0
		*AR0+ = AC0 || AC0 = *AR1+
	}
	AC0 = POPBOTH()
	return

	.end
;----------------------------------------------------------------
;End of file

