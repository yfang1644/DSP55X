;/*
; * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/ 
; * 
; * 
; *  Redistribution and use in source and binary forms, with or without 
; *  modification, are permitted provided that the following conditions 
; *  are met:
; *
; *    Redistributions of source code must retain the above copyright 
; *    notice, this list of conditions and the following disclaimer.
; *
; *    Redistributions in binary form must reproduce the above copyright
; *    notice, this list of conditions and the following disclaimer in the 
; *    documentation and/or other materials provided with the   
; *    distribution.
; *
; *    Neither the name of Texas Instruments Incorporated nor the names of
; *    its contributors may be used to endorse or promote products derived
; *    from this software without specific prior written permission.
; *
; *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
; *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
; *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
; *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
; *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
; *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
; *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
; *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
; *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
; *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; *
;*/

;***********************************************************
; Version 3.00.00                                           
;***********************************************************
;*********************************************************************
; Function:    cbrev32
; Processor:   C55xx
; Description: This function performs 32-bit bit-reversal of complex 
;                           data array x.
;              If x==y, in-place bit reversal is performed.
;
; Usage:    void cbrev32 (LDATA *x, LDATA *y, ushort nx)
;
; Limitations:
;   nx must be a power of 2 
;   nx >= 8
;
; Benchmarks:
;   Cycles:
;        x = y:   (9 * nx) + 43
;        x != y:  (4 * nx) + 30
;     nx =   8:  x=y:  115   x!=y:   62
;     nx =  16:  x=y:  187   x!=y:   94
;     nx =  32:  x=y:  331   x!=y:  158
;     nx =  64:  x=y:  619   x!=y:  286
;     nx = 128:  x=y: 1195   x!=y:  542
;     nx = 256:  x=y: 2347   x!=y: 1054
;     nx = 512:  x=y: 4651   x!=y: 2078
;   Code Size (in bytes):     116
;
; History:
;   Craig Leeds 05/25/2012:  optimized
;
;   delete in_place code. Fang Yuan 2014-08-18
;****************************************************************

                .mmregs  
                .cpl_on   

        .global _cbrev32

        .text
_cbrev32:

;//-----------------------------------------------------------------------------
;// Context Save
;//-----------------------------------------------------------------------------
;//-----------------------------------------------------------------------------
;// Initialize
;//-----------------------------------------------------------------------------

        BSET    AR1LC                       ; circular addressing for AR1
        .arms_off
        BCLR    ARMS                        ; reset ARMS bit

        MOV     T0, T1                      ; n in T1
        SFTL    T1, #1                      ; 2*n
        SFTL    T1, #1                      ; 4*n
        MOV     T1, mmap(BK03)              ; circular buffer size is 4*n 
        MOV     AR1, mmap(BSA01)            ; circular buffer offset      
 
        MOV     T0, T1                      ; n in T1
        SUB     #1, T1
        MOV     T1, BRC0                    ; BRC0 = n - 1

        MOV     #2, AR1                     ; output pointer (circular)

		SFTL    T0,#1

;//-----------------------------------------------------------------------------
;// Off-place bit reversal
;//-----------------------------------------------------------------------------
       || RPTBLOCAL NotOverlaidLoopEnd-1            
                MOV     dbl(*AR0+), AC0		; real part
             || ASUB    #2, AR1
                MOV     dbl(*AR0+), AC1		; imaginary part
             || MOV     AC0, dbl(*AR1+)
                MOV     AC1, dbl(*(AR1+T0B))  
NotOverlaidLoopEnd:

;//-----------------------------------------------------------------------------
;// Context Restore
;//-----------------------------------------------------------------------------
        BCLR    AR1LC                      ; restore linear addressing for AR1
        BSET    ARMS                       ; restore C environment
     || RET                                ; return   

        .end     

