; ___________________________________________________________________________
;|                                                                           |
;|   Function Name : Pow2()                                                  |
;|                                                                           |
;|     L_x = pow(2.0, exponant.fraction)         (exponant = interger part)  |
;|         = pow(2.0, 0.fraction) << exponant                                |
;|---------------------------------------------------------------------------|
;|  Algorithm:                                                               |
;|                                                                           |
;|   The function Pow2(L_x) is approximated by a table and linear            |
;|   interpolation.                                                          |
 |                                                                           |
;|   1- i = bit10-b15 of fraction,   0 <= i <= 31                            |
 |   2- a = bit0-b9   of fraction                                            |
;|   3- L_x = table[i]<<16 - (table[i] - table[i+1]) * a * 2                 |
;|   4- L_x = L_x >> (30-exponant)     (with rounding)                       |
;|___________________________________________________________________________|
;

	.ARMS_off                       ; enable assembler for ARMS=0
	.CPL_on                         ; enable assembler for CPL=1
	.mmregs                         ; enable mem mapped register names
	.include "c55xasm.i"
	.model  call=internal ; This function is recognized by name

	.data
table_pow2	.int	16384, 16743, 17109, 17484, 17867, 18258, 18658, 19066
			.int	19484, 19911, 20347, 20792, 21247, 21713, 22188, 22674
			.int	23170, 23678, 24196, 24726, 25268, 25821, 26386, 26964
			.int	27554, 28158, 28774, 29405, 30048, 30706, 31379, 32066
			.int	32767

	.text

	.def	_Pow2
_Pow2:
	PUSH (@ST1_L)					|| mmap()
	AC0 = T1						|| T1 = T0
	T1 -= #30						|| PSHBOTH(AC1)
	PSHBOTH(XAR0)					|| bit(ST1, #ST1_FRCT) = #1
	XAR0 = mar(*(#table_pow2))
	T0 = field_extract(AC0, #3f00h)	|| AC1 = AC0 << #21
	AC1 = AC1 & (#7fffh <<< #16)	|| PUSH(T2)
	T2 = *AR0(T0)					|| AR0 += #1
	HI(AC0) = T2					|| T2 -= *AR0(T0)
	AC0 = AC0 - (AC1 * T2)			|| T2 = POP()
	AC0 = AC0 <<C T1				|| XAR0 = POPBOTH()
	AC1 = POPBOTH()					|| bit(ST1, #ST1_SATD) = #1
	if(CARRY) execute(D_unit)		|| AC0 += #1
	@ST1_L = POP()					|| mmap()
	return


