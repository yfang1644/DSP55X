;****************************************************************
;
; memory copy in assembly languange
;
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.text

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

	.ref	_WATERMARK
	.def _DSP_watermark
; DSP_watermark(Uint8 *dst, Uint16 *src, Uint16 cnt);
_DSP_watermark:
	PSHBOTH(AC0)
	PUSH(T1)
	T0 -= #1
	BRC0 = T0
	T0 = *(#_WATERMARK)
	AC0 = uns(*AR1+)
	localrepeat {
		AC0 = AC0 >> #1 || T1 = field_extract(AC0, #0xff00)
		TC2 = bit(*(#20h), #15)
		T0 = TC2 \\ T0 \\ TC2 || *AR0+ = T1
		AC0 = CARRY \\ AC0 \\ TC2
		T1 = field_extract(AC0, #0x00ff)
		*AR0+ = T1 || AC0 = uns(*AR1+)
	}
	T1 = POP()
	AC0 = POPBOTH()
	*(#_WATERMARK) = T0				;; in case BRC0%16 != 0
	return


	.end
;----------------------------------------------------------------
;End of file

