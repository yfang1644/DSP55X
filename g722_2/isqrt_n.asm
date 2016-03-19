;___________________________________________________________________________
;|                                                                           |
;|   Function Name : Isqrt_n                                                 |
;|                                                                           |
;|       Compute 1/sqrt(value).                                              |
;|       if value is negative or zero, result is 1 (frac=7fffffff, exp=0).   |
;|---------------------------------------------------------------------------|
;|  Algorithm:                                                               |
;|                                                                           |
;|   The function 1/sqrt(value) is approximated by a table and linear        |
;|   interpolation.                                                          |
;|                                                                           |
;|   1- If exponant is odd then shift fraction right once.                   |
;|   2- exponant = -((exponant-1)>>1)                                        |
;|   3- i = bit25-b30 of fraction, 16 <= i <= 63 ->because of normalization. |
;|   4- a = bit10-b24                                                        |
;|   5- i -=16                                                               |
;|   6- fraction = table[i]<<16 - (table[i] - table[i+1]) * a * 2            |
;|___________________________________________________________________________|
;
	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names
	.include "c55xasm.i"

	.data
table_isqrt	.int	32767, 31790, 30894, 30070, 29309, 28602, 27945, 27330
			.int	26755, 26214, 25705, 25225, 24770, 24339, 23930, 23541
			.int	23170, 22817, 22479, 22155, 21845, 21548, 21263, 20988
			.int	20724, 20470, 20225, 19988, 19760, 19539, 19326, 19119
			.int	18919, 18725, 18536, 18354, 18176, 18004, 17837, 17674
			.int	17515, 17361, 17211, 17064, 16921, 16782, 16646, 16514
			.int	16384


	.text
	.def	_Isqrt_n_
_Isqrt_n_:
	PUSH(T0, T1)					|| T0 = *AR0
	T1 = T0 & #1
	if(T1 != #0) execute(D_unit)	|| AC0 = AC0 >> #1
	T0 -= #1
	T0 = T0 >> #1
	*AR0 = T0						|| AC0 = AC0 << #-9
	XAR0 = mar(*(#table_isqrt))
	PSHBOTH(AC1)					|| bit(ST1, #ST1_FRCT) = #1
	T0 = HI(AC0)					|| AC1 = field_extract(AC0, #0fffeh)
									; T0=b25-b3 , AC1=b10-b24
	AC1 = AC1 << #16				|| T0 -= #16
	T1 = *AR0(T0)					|| AR0 += #1
	HI(AC0) = T1					|| T1 -= *AR0(T0)

	AC0 = AC0 - (AC1 * T1)			|| AC1 = POPBOTH()
	T0, T1 = POP()					|| bit(ST1, #ST1_FRCT) = #0
	return

