;/****************************************************************************/
;/* AUTO_INIT()   v4.3.8 - Perform initialization of C variables.            */
;/* Copyright (c) 1996-2010  Texas Instruments Incorporated                  */
;/****************************************************************************/

	.include "c55xasm.i"
	.model  call=internal
	.mmregs
	.cpl_on
	.arms_on
        .asg !(__LARGE_MODEL__ | __HUGE_MODEL__), __SMALL_MODEL__

; Symbols defined by the Linker
	.ref	cinit
	.ref	___bss__
	.ref	___pinit__

; Symbols defined here
	.def _auto_init

  .if .PHASE3
	.noremark 5572    ; safe with respect to Silicon Exception CPU_42
	.noremark 5573    ; safe with respect to Silicon Exception CPU_43
  .endif

;****************************************************************************/
; C VARIABLE INITIALIZATION TABLE FORMAT                                    */
;                                                                           */
;      .field <reserved>,                      2                            */
;      .field <length of init data in words>, 14                            */
;      .field <address of variable>,          24                            */
;      .field <address space indicator>,       8                            */
;      .word  <init data>                                                   */
;      .word  ...                                                           */
;                                                                           */
; The table is terminated with a record with data length zero.              */
;                                                                           */
; The first two bits of the word containing the data length are reserved,   */
; and must be 00. If you create your own cinit records and use other than   */
; your code may be incompatible with a future compiler.                     */
;                                                                           */
;****************************************************************************/


;*****************************************************************************
; auto_init() - C/C++ auto-initialization. Uses .cinit sections to 
;               initialize variables. Uses .pinit sections to call
;               static initialization functions
;*****************************************************************************
_auto_init:	.asmfunc

;----------------------------------------------------------------------
; Allocate local frame. All except C55+ byte model require odd amount
; for even stack alignment. 
;----------------------------------------------------------------------
  .if (.PHASE2 | __SMALL_MODEL__)
	SP = SP - #3
  .elseif !__TMS320C55X_PLUS_BYTE__
	SP = SP - #1
  .endif

;----------------------------------------------------------------------
; Load address register with 23-bit address of cinit table
;----------------------------------------------------------------------
  .if __TMS320C55X_PLUS_BYTE__
        XAR0  = #(cinit & 0xffffff)
  .elseif .PHASE3 | __TMS320C55X_PLUS__
        XAR0  = #(cinit & 0x7fffff)
  .else
        MDP05 = #((cinit >> 16) & 0x7f)
        AR0   = #(cinit & 0xffff)
  .endif

;----------------------------------------------------------------------
;  If table pointer is -1 then exit
;----------------------------------------------------------------------
	AR7 = ~AR0
	if (AR7 == #0) goto PINIT

;----------------------------------------------------------------------
; Copy data from .cinit section to .bss section
;   AR2 = data length of current record (0 => finished)
;   AC0 = 23-bit variable address in .bss section
;   AC1 = Address space indicator (0 => DATA, 1 => IO)
;----------------------------------------------------------------------
LOOP:
        AR2 = *AR0+
        if (AR2 == #0) goto PINIT

        AC0 = uns(*AR0+) << #8
||	AR2 = AR2 - #1
	AC1 = uns(*AR0+)
||	CSR = AR2
	AC0 = AC0 | (AC1 <<< #-8)
	AC1 = AC1 & #0xff	

;----------------------------------------------------------------------
; Load extended address register with 23-bit address of variable
;----------------------------------------------------------------------
  .if .PHASE3 | __TMS320C55X_PLUS__
	XAR6  = AC0
  .else
	MDP67 = @AC0_H || mmap()
	AR6   = AC0
  .endif	

;----------------------------------------------------------------------
; Do the appropriate copy operation
;----------------------------------------------------------------------
	if (AC1 != #0) goto PORTMEMCOPY

DATAMEMCOPY:
  .if __TMS320C55X_PLUS_BYTE__
	repeat(CSR)
	    byte(*AR6+) = byte(*AR0+)
	mar(XAR0 + #1) || bit(AR0, @0) = #0
  .else
	repeat(CSR)
	    *AR6+ = *AR0+
  .endif
	goto LOOP

PORTMEMCOPY:
  .if __TMS320C55X_PLUS_BYTE__
	AR2 = AR2 >> #1
	CSR = AR2
  .endif
	repeat(CSR)
            *AR6+ = *AR0+ || writeport()
	goto LOOP


PINIT:		
;----------------------------------------------------------------------
; Call initialization routines found in pinit table
;----------------------------------------------------------------------

;----------------------------------------------------------------------
; Reset address extension value
;----------------------------------------------------------------------
  .if .PHASE2
	MDP05 = #0
  .elseif __SMALL_MODEL__
	XAR0 = #___bss__
  .endif

;----------------------------------------------------------------------
; Load extended address register with 23-bit address of pinit table
;----------------------------------------------------------------------
  .if __TMS320C55X_PLUS_BYTE__
        XAR6  = #(___pinit__ & 0xffffff)
  .elseif .PHASE3 | __TMS320C55X_PLUS__
        XAR6  = #(___pinit__ & 0x7fffff)
  .else
        MDP67 = #((___pinit__ >> 16) & 0x7f)
        AR6   = #(___pinit__ & 0xffff)
  .endif

;----------------------------------------------------------------------
;  If table pointer is -1 then exit
;----------------------------------------------------------------------
	AR7 = ~AR6
	if (AR7 == #0) goto EXIT
		
LOOP2:	
;----------------------------------------------------------------------
; AC0 = address of initialization routine to call (0 => finished)
;----------------------------------------------------------------------
        AC0 = dbl(*AR6+)
        if (AC0 == #0) goto EXIT

;----------------------------------------------------------------------
; Call Initialization Function. Save/restore address extension values
; around the call to compiled code.   
;----------------------------------------------------------------------
  .if .PHASE2
        *SP(#0) = MDP67
	MDP67 = #0

        call AC0

	MDP67 = *SP(#0) 

  .elseif __SMALL_MODEL__
	dbl(*SP(#0)) = XAR6
	nop   ; exception CPU_7
	nop   ; exception CPU_7
	nop   ; exception CPU_7
	nop   ; exception CPU_7
	XAR6 = #___bss__

        call AC0

	XAR6 = dbl(*SP(#0))
  .else
        call AC0
  .endif

	goto LOOP2	

EXIT:
;----------------------------------------------------------------------
; Reset address extension value
;----------------------------------------------------------------------
  .if .PHASE2
	MDP67 = #0
  .elseif __SMALL_MODEL__
	XAR6 = #___bss__
  .endif

;----------------------------------------------------------------------
; Deallocate local frame         
;----------------------------------------------------------------------
  .if (.PHASE2 | __SMALL_MODEL__)
	SP = SP + #3
  .elseif !__TMS320C55X_PLUS_BYTE__
	SP = SP + #1
  .endif

        return
	.endasmfunc

	.end
