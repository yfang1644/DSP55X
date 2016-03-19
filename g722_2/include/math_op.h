/*--------------------------------------------------------------------------*
 *                         MATH_OP.H	                                    *
 *--------------------------------------------------------------------------*
 *       Mathematical operations					                        *
 *--------------------------------------------------------------------------*/

Word32 Isqrt_n(
     Word32  frac,     /* (i/o) Q31: normalized value (1.0 < frac <= 0.5) */
     Word16 * exp       /* (i/o)    : exponent (value = frac x 2^exponent) */
);

Word32 Pow2(            /* (o) Q0  : result     (range: 0<=val<=0x7fffffff) */
     Word16 exponant,   /* (i) Q0  : Integer part.    (range: 0<=val<=30)   */
     Word16 fraction    /* (i) Q15 : Fractionnal part.(range: 0.0<=val<1.0) */
);

Word32 Dot_product(          /* (o) Q31:                                   */
     Word16 x[],             /* (i) 12bits: x vector                       */
     Word16 y[],             /* (i) 12bits: y vector                       */
     Word16 lg               /* (i)    : vector length                     */
);

Word32 Dot_product12(        /* (o) Q31: normalized result (1 < val <= -1) */
     Word16 x[],             /* (i) 12bits: x vector                       */
     Word16 y[],             /* (i) 12bits: y vector                       */
     Word16 lg,              /* (i)    : vector length                     */
     Word16 * exp            /* (o)    : exponent of result (0..+30)       */
);

void Log2 (
    Word32 L_x,         /* (i) : input value                                 */
    Word16 *exponent,   /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    Word16 *fraction    /* (o) : Fractional part of Log2. (range: 0<=val<1) */
);

Word16 Random(Word16 * seed);

