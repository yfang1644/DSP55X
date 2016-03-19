;----------------------------------------------------------------------*
;                         upd_tar.c                                    *
;----------------------------------------------------------------------*
; Update the target vector for codebook search.                        *
;----------------------------------------------------------------------*
;
;void Updt_tar(
;     Word16 * x,      /* (i) Q0  : old target (for pitch search)     */
;     Word16 * x2,     /* (o) Q0  : new target (for codebook search)  */
;     Word16 * y,      /* (i) Q0  : filtered adaptive codebook vector */
;     Word16 gain,     /* (i) Q14 : adaptive codebook gain            */
;     Word16 L         /* (i)     : subframe size                     */
;)
;{
;    Word16 i, tmp;
;    Word32 L_tmp;
;    for (i = 0; i < L; i++)
;    {
;        L_tmp = (Word32)x[i] << 15;
;        L_tmp = L_msu(L_tmp, y[i], gain);
;        L_tmp = L_shl(L_tmp, 1);
;        x2[i] = (Word16)(L_tmp >> 16);
;    }
;
;   return;
;}

	.ARMS_off                       ; enable assembler for ARMS=0
	.CPL_on                         ; enable assembler for CPL=1
	.mmregs                         ; enable mem mapped register names

	.text

	.def    _Updt_tar
_Updt_tar:
	T1 -= #1
	BRC0 = T1
	bit(ST1, #ST1_FRCT) = #1 || PSHBOTH(AC0)

	localrepeat {
		AC0 = (*AR0+) << #15
		AC0 = AC0 - ((*AR2+) * T0)
		*AR1+ = HI(saturate(AC0 << #1))
	}

	bit(ST1, #ST1_FRCT) = #0 || AC0 = POPBOTH()
	return

;OK 2011-12-02
;modified 2012-01-04
