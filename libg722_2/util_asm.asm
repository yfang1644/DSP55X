;--------------------------------------------------------------------*
;                          util.c                                    *
;--------------------------------------------------------------------*
;  Vector routines                                                   *
;--------------------------------------------------------------------*
;--------------------------------------------------------------------*
;  Function  Set zero()                                              *
;            ~~~~~~~~~~                                              *
;  Set vector x[] to zero                                            *
;--------------------------------------------------------------------*
;
;void Set_zero(
;     Word16 x[],                  /* (o)    : vector to clear     */
;     Word16 L                     /* (i)    : length of vector    */
;)
;{
;    Word16 i;
;
;    for (i = 0; i < L; i++)
;    {
;        x[i] = 0;
;    }
;
;    return;
;}
;
;
;--------------------------------------------------------------------*
;  Function  Copy:                                                   *
;            ~~~~~                                                   *
;  Copy vector x[] to y[]                                            *
;--------------------------------------------------------------------*
;
;void Copy(
;     Word16 x[],                         /* (i)   : input vector   */
;     Word16 y[],                         /* (o)   : output vector  */
;     Word16 L                            /* (i)   : vector length  */
;)
;{
;    Word16 i;
;
;    for (i = 0; i < L; i++)
;    {
;        y[i] = x[i];
;    }
;
;    return;
;}
;
;
	.ARMS_off                       ; enable assembler for ARMS=0
	.CPL_on                         ; enable assembler for CPL=1
	.mmregs                         ; enable mem mapped register names

	.text

    .def    _Set_zero
_Set_zero:
	T0 -= #1
	CSR = T0

	repeat(CSR)
		*AR0+ = #0

	return

	.def	_Copy
_Copy:
	T0 -= #1
	CSR = T0

	repeat(CSR)
		*AR1+ = *AR0+

	return

;OK 2011-12-01
