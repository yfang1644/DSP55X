;*************************************************************************
;*
;*   FUNCTION:   Log2()
;*
;*   PURPOSE:   Computes log2(L_x),  where   L_x is positive.
;*              If L_x is negative or zero, the result is 0.
;*
;*   DESCRIPTION:
;*        normalizes L_x and then calls Log2_norm().
;*
;*************************************************************************/
	.data
table_log	.int        0,  1455,  2866,  4236,  5568,  6863,  8124,  9352
			.int	10549, 11716, 12855, 13967, 15054, 16117, 17156, 18172
			.int	19167, 20142, 21097, 22033, 22951, 23852, 24735, 25603
			.int	26455, 27291, 28113, 28922, 29716, 30497, 31266, 32023
			.int	32767

	.text
	.def	_Log2_
_Log2_:
	if(AC0 <= #0) execute(D_unit)	|| *AR0 = LO(AC0), *AR1 = HI(AC0)
	if(AC0 <= #0) return

	PUSH(T0, T1)					|| bit(ST1, #ST1_FRCT) = #1
	AC0 = mant(AC0), T1 = -exp(AC0)	|| PSHBOTH(AC1)
	AC0 = AC0 << #-9				|| T1 += #30
	T0 = HI(AC0)					|| AC1 = field_extract(AC0, #0fffeh)
	AC1 = AC1 << #16				|| *AR0 = T1
	T0 -= #32
	XAR0 = mar(*(#table_log))
	T1 = *AR0(T0)					|| AR0 += #1
	HI(AC0) = T1					|| T1 -= *AR0(T0)
	AC0 = AC0 - (AC1 * T1)			|| AC1 = POPBOTH()
	*AR1 = HI(AC0)					|| T0, T1 = POP()
	bit(ST1, #ST1_FRCT) = #0		|| return


