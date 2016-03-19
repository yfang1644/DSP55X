;***************************************************************************
;log2
;
; Calculate Log2 and scale the signal:
;
; ilog2(Word32 in) = -1024*log10(in * 2^-31)/log10(2), where in = [1, 2^31-1]
;
; input   output
; 32768   16384
; 1       31744
;
; When input is in the range of [1,2^16], max error is 0.0380%.
;
;***************************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.noremark	5673, 5584
	.text

	.def	_ilog2
_ilog2:
	AC0 = T0 || PSHBOTH(AC0)
    if(T0 == #0) execute(D_unit) || AC0 = #1
	bit(ST1, #ST1_FRCT) = #1
	AC0 = mant(AC0), T1 = -exp(AC0)

	repeat (#3)
		AC0 = AC0 * AC0

	AC0 = mant(AC0), T0 = -exp(AC0)

	AC0 = AC0 << #-24
	AC0 = AC0 + (*(#21h) << #10)	;; T1
	AC0 = AC0 + (*(#20h) << #6)

	T0 = #127 || T1 = LO(AC0)
	T0 = T0 - T1 || AC0 = POPBOTH()

	bit(ST1, #ST1_FRCT) = #0 ||
	return

;OK 2011-12-01
;modified 2012-01-11