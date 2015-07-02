;/****************************************************************************/
;/*  divs  v4.4.1                                                            */
;* 
;* Copyright (c) 1997-2012 Texas Instruments Incorporated
;* http://www.ti.com/ 
;* 
;*  Redistribution and  use in source  and binary forms, with  or without
;*  modification,  are permitted provided  that the  following conditions
;*  are met:
;* 
;*     Redistributions  of source  code must  retain the  above copyright
;*     notice, this list of conditions and the following disclaimer.
;* 
;*     Redistributions in binary form  must reproduce the above copyright
;*     notice, this  list of conditions  and the following  disclaimer in
;*     the  documentation  and/or   other  materials  provided  with  the
;*     distribution.
;* 
;*     Neither the  name of Texas Instruments Incorporated  nor the names
;*     of its  contributors may  be used to  endorse or  promote products
;*     derived  from   this  software  without   specific  prior  written
;*     permission.
;* 
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;/****************************************************************************/
	.model  call=internal ; This function is recognized by name 
			      ; in the compiler.

; fractional (Q15) 16-bit integer divide
;
; arg1:   ?: T0
; arg2:   ?: T1
; return: ?: T0 (Q15)
;

	.sect	".text"
	.global	_divs
_divs:	.asmfunc
		if (T0 == #0) return	; note: return value is zero

	.if __TMS320C55X_PLUS__
		TC1 = (T0 == T1)
	.else
		TC1 = (T0 == T1)
		|| nop					; avoid Silicon Exception CPU_24
	.endif
		if (TC1) execute(D_Unit) || T0 = #32767 
		if (TC1) return

		AC0 = T0 || BRC0 = #14	; T0 last use as input
		AC1 = T1 || T0 = #0
	.if __TMS320C55X_PLUS__
		localrepeat {
			AC0 = AC0 << #1           ||  T0 = T0 << #1
			TC1 = (AC0 < AC1)	
			if (!TC1) execute(D_Unit) ||  AC0 = ssat(AC0 - AC1)
			if (!TC1) execute(D_Unit) ||  T0 = T0 + #1
		}
	.else
		localrepeat {
			AC0 = AC0 << #1           ||  T0 = T0 << #1
			TC1 = (AC0 < AC1)         ||  bit(ST1, #ST1_SATD) = #1
			if (!TC1) execute(D_Unit) ||  AC0 = AC0 - AC1
			if (!TC1) execute(D_Unit) ||  T0 = T0 + #1
			bit(ST1, #ST1_SATD) = #0
		}
	.endif
		return

	.endasmfunc

