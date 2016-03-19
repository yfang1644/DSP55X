; fractional (Q15) unsigned 16-bit integer divide
;
; arg1:   ?: T0 >= 0
; arg2:   ?: T1 > 0
; arg1 < arg2 && arg1 > (arg2/2)
; return: ?: T0 (Q15) = arg1 / arg2
;

	.sect	".text"
	.global	_div_s
_div_s:.asmfunc
		if (T0 == #0) return	; note: return value is zero

		TC1 = (T0 == T1)
		if (TC1) execute(D_Unit) || T0 = #32767 
		if (TC1) return

		HI(AC0) = T0 || repeat (#(15-1))
			subc(*(#21h), AC0, AC0)

		T0 = AC0 || return

	.endasmfunc

