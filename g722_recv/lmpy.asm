******************************************************************************
* lmpy.asm  v4.3.8
* Copyright (c) 1997-2010 Texas Instruments Incorporated
******************************************************************************
	.include "c55xasm.i"
	.mmregs
	.cpl_on
	.arms_on
	.model  call=internal ; This function is recognized by name 
			      ; in the compiler.

	.noremark 5673        ; The shifts in this file avoid SE CPU_89


*****************************************************************************
* _LMPY() - Multiply two 32-bit longs into a long.
*
* WARNING!  Changes to register defs in this function must be reflected in
* the constraint class MPYLI_SET in ld3.md
*****************************************************************************
	.global	I$$LMPY
	.global	__mpyli
	.noremark 5108
		
; inputs in AC0 and AC1
; AC0 -> IHI ILO
; AC1 -> JHI JLO
; for the computation the inputs are:
; IHI -> AC0_H
; ILO -> T0
; JHI -> AC1_H
; JLO -> SP(#0)
;
; For Ryujin the computation inputs are:
; IHI -> AC0.H
; ILO -> AC0.H
; JHI -> AC1.H
; JLO -> AC1.L
;
; result in AC0

	.if __TMS320C55X_PLUS_BYTE__
FRAME_SIZE	.set	2
	.else
FRAME_SIZE	.set	1
	.endif

I$$LMPY:	.asmfunc
__mpyli:
******************************************************************************
* ALLOCATE FRAME
******************************************************************************
  .if ! __TMS320C55X_PLUS__
	push(AC1)               ; Save off AC1_L (i.e. JLO) so it's available
	                        ; as an smem later on.  Note that this 
	                        ; also aligns the stack.

 ||     T0 = AC0		; ILO
  .endif

******************************************************************************
* A 32X32 MULTIPLY WITH A 64-BIT RESULT WOULD BE CALCULATED: 
*                                                            
*                IHI ILO                                     
*             X  JHI JLO                                     
*             ----------                                     
*             ILO * JLO                                      
*       JLO * IHI                                            
*       ILO * JHI                                            
* IHI * JHI                                                  
* ----------------------                                     
*     64-bit result                                          
*                                                            
* BUT WE NEED ONLY THE LOWER 32-BITS OF THIS CALCULATION.  THEREFORE   
* THE IHI * JHI CALCULATION ISN'T DONE AT ALL AND THE UPPER HALF OF THE
* JLO * IHI AND ILO * JHI CALCULATIONS IS THROWN AWAY.  ALSO, THE      
* ILO * JLO MULTIPLY MUST BE UNSIGNED, BUT THE SIGNNESS OF THE OTHER   
* MULTIPLIES DOESN'T MATTER SINCE THE DIFFERENCE ALWAYS APPEARS IN THE 
* UPPER 16-BITS.                                                       
******************************************************************************
  .if   __TMS320C55X_PLUS__
 	AC2 = AC0.H * AC1.L	         ; AC2 = IHI * JLO
 	AC2 = AC2 + (AC1.H * AC0.L)      ; AC2 = (IHI * JLO) + (JHI * ILO)
 	AC0 = uns(AC0.L) * uns(AC1.L)    ; AC0 = ILO * JLO
 	AC0.H = AC0.H + AC2.L            ; final result

 ||     return
	
  .else	

	AC0 = AC0 * *SP(#0)	      	 ; AC0 = IHI * JLO
	AC1 = (AC1 * T0) + AC0	      	 ; AC1 = (JHI * ILO) + (IHI * JLO)
	AC0 = uns(T0 * *SP(#0))	      	 ; AC0 = ILO * JLO
	AC0 = AC0 + (AC1 << #16)      	 ; final result  
		
||	SP = SP + #FRAME_SIZE  	     ; deallocate frame

        return
  .endif
	.endasmfunc
        .end
