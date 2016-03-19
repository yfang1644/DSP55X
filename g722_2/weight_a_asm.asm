; *-------------------------------------------------------------------*
; * Weighting of LPC coefficients.									  *
; *   ap[i]  =  a[i] * (gamma ** i)									  *
; *-------------------------------------------------------------------*/
;
;void Weight_a(
;     Word16 a[],     /* (i) Q12 : a[m+1]  LPC coefficients             */
;     Word16 ap[],    /* (o) Q12 : Spectral expanded LPC coefficients   */
;     Word16 gamma,   /* (i) Q15 : Spectral expansion factor.           */
;     Word16 m        /* (i)     : LPC order.                           */
;)
;{
;    Word16 i, fac;
;
;    ap[0] = a[0];
;    fac = gamma;
;    for (i = 1; i < m; i++)
;    {
;        ap[i] = mult_r(a[i], fac);
;        fac = mult_r(fac, gamma);
;    }
;    ap[m] = mult_r(a[m], fac);
;
;    return;
;}
;
	.ARMS_off                       ; enable assembler for ARMS=0
	.CPL_on                         ; enable assembler for CPL=1
	.mmregs                         ; enable mem mapped register names

	.noremark   5673, 5584
	.text

	.def    _Weight_a
_Weight_a:
	T1 -= #2
	BRC0 = T1						; T1 = loop counter( 0 -- m )

	bit(ST1, #ST1_FRCT) = #1 || PSHBOTH(AC0)
	PSHBOTH(AC1)

	*AR1+ = *AR0+
	HI(AC1) = T0
	localrepeat {
		AC0 = (*AR0+) * AC1
		*AR1+ = HI(rnd(AC0)) ||
		AC1 = rnd(AC1 * T0)			; T0 = gamma
	}
	AC0 = (*AR0+) * AC1
	*AR1+ = HI(rnd(AC0)) || AC0 = POPBOTH()

	bit(ST1, #ST1_FRCT) = #0 ||	AC1 = POPBOTH() 
	return

;modified 2012-01-09