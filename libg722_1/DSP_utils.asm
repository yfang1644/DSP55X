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
; set zero to buf[cnt]
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

	.def _DSP_add
; DSP_add(Word16 *ptr1, Word16 *ptr2, Word16 cnt)
; ptr1 = ptr1 + ptr2
_DSP_add:
	PUSH (@ST1_L) || mmap()
	bit(ST1, #ST1_SATD) = #1
	T0 -= #1
	BRC0 = T0 || T0 = *AR1+
	localrepeat{
		T0 += *AR0
		*AR0+ = T0 || T0 = *AR1+
	}
	@ST1_L = POP() || mmap()
	return

	.def _DSP_mac
; Word16 DSP_mac(Word16 *ptr1, Word16 *ptr2, Word16 cnt)
; sum = dotproduct(ptr1, ptr2)
; return round16(sum)
_DSP_mac:
	PUSH (@ST1_L) || mmap()
	bit(ST1, #ST1_SATD) = #1		; saturate
	bit(ST1, #ST1_FRCT) = #1		; shift left by 1 bit
	PSHBOTH(AC0) ||	T0 -= #1
	AC0 = #0 || CSR = T0
	repeat(CSR)
		AC0 = AC0 + ((*AR0+)*(*AR1+))

	*(#20h) = HI(saturate(rnd(AC0)))	; T0 = rnd(AC0)
	AC0 = POPBOTH()
	@ST1_L = POP() || mmap()
	return

	.end
;----------------------------------------------------------------
;End of file

