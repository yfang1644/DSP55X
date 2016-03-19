;***************************************************************************
;___________________________________________________________________________
;                                                                           |
; 32bit arithmetic operators.                                               |
;                                                                           |
; $Id $                                                                     |
;___________________________________________________________________________|
;
; File:        oper_32b_asm.asm
; Processor:   C55xx
;
; Usage: void func(Word32 var1, Word32 var2, Word16 var3....)
;     First Word16 as T0, second as T1,...(third may be AR0?)
;     First Word32 as AC0, second as AC1, third as AC2
;
;     return Word32 in AC0
;     return Word16 in T0
;****************************************************************

	.ARMS_off						; enable assembler for ARMS=0
	.CPL_on							; enable assembler for CPL=1
	.mmregs							; enable mem mapped register names

	.noremark	5673

	.text

	.def	_L_Extract
;****************************************************************************
;                                                                            *
;  Function L_Extract()                                                     *
;                                                                           *
;  Extract from a 32 bit integer two 16 bit DPF.                            *
;                                                                           *
;  Arguments:                                                               *
;                                                                           *
;   L_32      : 32 bit integer.                                             *
;               0x8000 0000 <= L_32 <= 0x7fff ffff.                         *
;   hi        : b16 to b31 of L_32                                          *
;   lo        : (L_32 - hi<<16)>>1                                          *
;****************************************************************************
_L_Extract:
	*AR0 = HI(AC0) || AC0 = AC0 >> #1
	AC0 = AC0 & 0x7fff
	*AR1 = AC0 || return

	.def	_L_Comp
;****************************************************************************
;                                                                           *
;  Function L_Comp(short hi, short lo)                                      *
;                                                                           *
;  Compose from two 16 bit into a 32 bit Q31.                               *
;                                                                           *
;     L_32 = hi<<16 + lo<<1                                                 *
;                                                                           *
;  Arguments:                                                               *
;                                                                           *
;   hi        msb                                                           *
;   lo        lsf (with sign)                                               *
;                                                                           *
;   Return Value :                                                          *
;                                                                           *
;             32 bit long signed integer (Word32) whose value falls in the  *
;             range : 0x8000 0000 <= L_32 <= 0x7fff fff0.                   *
;                                                                           *
;****************************************************************************
_L_Comp:
	HI(AC0) = T0
	AC0 = AC0 + (T1 * #2) || return

;----------------------------------------------------------------
;End of file

