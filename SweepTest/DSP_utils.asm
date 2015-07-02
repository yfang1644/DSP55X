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

	.end
;----------------------------------------------------------------
;End of file

