/*___________________________________________________________________________
 |                                                                           |
 |  This file contains mathematic operations in fixed point.                 |
 |                                                                           |
 |  Isqrt()              : inverse square root (16 bits precision).          |
 |  Pow2()               : 2^x  (16 bits precision).                         |
 |  Log2()               : log2 (16 bits precision).                         |
 |  Dot_product()        : scalar product of <x[],y[]>                       |
 |  Random()             : Signed 16 bits random generator.                  |
 |                                                                           |
 |  These operations are not standard double precision operations.           |
 |  They are used where low complexity is important and the full 32 bits     |
 |  precision is not necessary. For example, the function Div_32() has a     |
 |  24 bits precision which is enough for our purposes.                      |
 |                                                                           |
 |  In this file, the values use theses representations:                     |
 |                                                                           |
 |  Word32 L_32     : standard signed 32 bits format                         |
 |  Word16 hi, lo   : L_32 = hi<<16 + lo<<1  (DPF - Double Precision Format) |
 |  Word32 frac, Word16 exp : L_32 = frac << exp-31  (normalised format)     |
 |  Word16 int, frac        : L_32 = int.frac        (fractional format)     |
 |___________________________________________________________________________|
*/

#include "typedef.h"
#include "math_op.h"
#include "basic_op.h"

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Isqrt_n                                                 |
 |                                                                           |
 |       Compute 1/sqrt(value).                                              |
 |       if value is negative or zero, result is 1 (frac=7fffffff, exp=0).   |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The function 1/sqrt(value) is approximated by a table and linear        |
 |   interpolation.                                                          |
 |                                                                           |
 |   1- If exponant is odd then shift fraction right once.                   |
 |   2- exponant = -((exponant-1)>>1)                                        |
 |   3- i = bit25-b30 of fraction, 16 <= i <= 63 ->because of normalization. |
 |   4- a = bit10-b24                                                        |
 |   5- i -=16                                                               |
 |   6- fraction = table[i]<<16 - (table[i] - table[i+1]) * a * 2            |
 |___________________________________________________________________________|
*/
Word16 table_isqrt[49] =
{
    32767, 31790, 30894, 30070, 29309, 28602, 27945, 27330, 26755, 26214,
    25705, 25225, 24770, 24339, 23930, 23541, 23170, 22817, 22479, 22155,
    21845, 21548, 21263, 20988, 20724, 20470, 20225, 19988, 19760, 19539,
    19326, 19119, 18919, 18725, 18536, 18354, 18176, 18004, 17837, 17674,
    17515, 17361, 17211, 17064, 16921, 16782, 16646, 16514, 16384
};

Word32 Isqrt_n(
     Word32  frac,      /* (i/o) Q31: normalized value (1.0 < frac <= 0.5) */
     Word16 * exp        /* (i/o)    : exponent (value = frac x 2^exponent) */
)
{
    Word16 i, a, tmp;

    i = (*exp) & 1;                      /* If exponant odd -> shift right */
    frac = frac >> i;

    a = *exp - 1;
    *exp = a >> 1;

    frac = frac >> 10;
    i = (Word16)(frac >> 15);             /* Extract b25-b31 */
    a = (Word16)(frac & (Word16)0x7fff);   /* Extract b10-b24 */

    i = i - 16;
    frac = (Word32)table_isqrt[i] << 16;   /* table[i] << 16         */
    tmp = table_isqrt[i] - table_isqrt[i + 1];  /* table[i] - table[i+1]) */
    frac = L_msu(frac, a, tmp);          /* frac -=  tmp*a*2       */

    return frac;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Pow2()                                                  |
 |                                                                           |
 |     L_x = pow(2.0, exponant.fraction)         (exponant = interger part)  |
 |         = pow(2.0, 0.fraction) << exponant                                |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The function Pow2(L_x) is approximated by a table and linear            |
 |   interpolation.                                                          |
 |                                                                           |
 |   1- i = bit10-b15 of fraction,   0 <= i <= 31                            |
 |   2- a = bit0-b9   of fraction                                            |
 |   3- L_x = table[i]<<16 - (table[i] - table[i+1]) * a * 2                 |
 |   4- L_x = L_x >> (30-exponant)     (with rounding)                       |
 |___________________________________________________________________________|
*/
Word16 table_pow2[33] =
{
    16384, 16743, 17109, 17484, 17867, 18258, 18658, 19066, 19484, 19911,
    20347, 20792, 21247, 21713, 22188, 22674, 23170, 23678, 24196, 24726,
    25268, 25821, 26386, 26964, 27554, 28158, 28774, 29405, 30048, 30706,
    31379, 32066, 32767
};

Word32 Pow2(             /* (o) Q0  : result     (range: 0<=val<=0x7fffffff) */
     Word16 exponant,    /* (i) Q0  : Integer part.    (range: 0<=val<=30)   */
     Word16 fraction     /* (i) Q15 : Fractionnal part.(range: 0.0<=val<1.0) */
)
{
    Word16 exp, i, a, tmp;
    Word32 L_x;
    Word32 bit;

    L_x = (Word32)fraction << 5;           /* L_x = fraction<<5           */
    i = fraction >> 10;                    /* Extract b10-b16 of fraction */
    a = (Word16)(L_x & (Word16)0x7fff);    /* Extract b0-b9   of fraction */

    L_x = (Word32)table_pow2[i] << 16;      /* table[i] << 16        */
    tmp = table_pow2[i] - table_pow2[i + 1];  /* table[i] - table[i+1] */
    L_x = L_msu(L_x, a, tmp);               /* L_x -= tmp*a*2        */

    exp = sub(30, exponant);
    if (exp > 0)
    {
        bit = (Word32)1 << (exp - 1);
        L_x += bit;
        L_x = L_x >> exp;
    }
    else
        L_x = L_shl(L_x, -exp);

    return (L_x);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Dot_product()                                           |
 |                                                                           |
 |       Compute scalar product of <x[],y[]> using MAC.                      |
 |                                                                           |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |       dot_product = sum(x[i]*y[i])     i=0..N-1                           |
 |___________________________________________________________________________|
*/

Word32 Dot_product_(         /* (o) Q31:                                   */
     Word16 x[],             /* (i) 12bits: x vector                       */
     Word16 y[],             /* (i) 12bits: y vector                       */
     Word16 lg               /* (i)    : vector length                     */
)
{
    Word16 i;
    Word32 L_sum;

    L_sum = 1L;
    for (i = 0; i < lg; i++)
        L_sum = L_mac(L_sum, x[i], y[i]);

    return (L_sum);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Dot_product12()                                         |
 |                                                                           |
 |       Compute scalar product of <x[],y[]> using accumulator.              |
 |                                                                           |
 |       The result is normalized (in Q31) with exponent (0..30).            |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |       dot_product = sum(x[i]*y[i])     i=0..N-1                           |
 |___________________________________________________________________________|
*/

Word32 Dot_product12_(       /* (o) Q31: normalized result (1 < val <= -1) */
     Word16 x[],             /* (i) 12bits: x vector                       */
     Word16 y[],             /* (i) 12bits: y vector                       */
     Word16 lg,              /* (i)    : vector length                     */
     Word16 * exp            /* (o)    : exponent of result (0..+30)       */
)
{
    Word16 i;
    Word32 L_sum;

    L_sum = 1L;
    for (i = 0; i < lg; i++)
        L_sum = L_mac(L_sum, x[i], y[i]);

    /* Normalize acc in Q31 */
//    *exp = -norm_l0(L_sum);
//    L_sum = L_shl(L_sum, -(*exp));
	*exp = exp_mant0(&L_sum);
    return (L_sum);
}

/******************************************************************************
*
*      Purpose          : Table for routine Log2().
*      $Id $
*
******************************************************************************/
Word16 table_log[33] =
{
    0, 1455, 2866, 4236, 5568, 6863, 8124, 9352, 10549, 11716,
    12855, 13967, 15054, 16117, 17156, 18172, 19167, 20142, 21097, 22033,
    22951, 23852, 24735, 25603, 26455, 27291, 28113, 28922, 29716, 30497,
    31266, 32023, 32767
};

/*************************************************************************
 *
 *   FUNCTION:   Log2()
 *
 *   PURPOSE:   Computes log2(L_x),  where   L_x is positive.
 *              If L_x is negative or zero, the result is 0.
 *
 *   DESCRIPTION:
 *        normalizes L_x and then calls Log2_norm().
 *
 *************************************************************************/
void Log2 (
    Word32 L_x,         /* (i) : input value                                 */
    Word16 *exponent,   /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    Word16 *fraction    /* (o) : Fractional part of Log2. (range: 0<=val<1) */
)
{
    Word16 i, a, tmp;
    Word32 L_y;

	L_y = L_x;
    if (L_y <= (Word32) 0)
    {
        *exponent = 0;
        *fraction = 0;
        return;
    }

    tmp = exp_mant0 (&L_y);
    *exponent = 30 + tmp;

    L_y = L_y >> 10;
    i = (Word16) (L_y >> 15);           /* Extract b25-b31 */
    a = (Word16) (L_y & 0x7fff);        /* Extract b10-b24 of fraction */

    i = i - 32;

    L_y = (Word32)table_log[i] << 16;            /* table[i] << 16        */
    tmp = table_log[i] - table_log[i + 1];       /* table[i] - table[i+1] */
    L_y = L_msu(L_y, tmp, a);               /* L_y -= tmp*a*2        */

    *fraction = (Word16) (L_y >> 16);

    return;
}


/*-------------------------------------------------------------------*
 *                         Random									 *
 *-------------------------------------------------------------------*
 * Signed 16 bits random generator.								     *
 *-------------------------------------------------------------------*/

Word16 Random(Word16 * seed)
{
    /* static Word16 seed = 21845; */
    Word16 tmp;

    tmp = (*seed) * 31821;
    *seed = tmp + 13849;
//    *seed = (Word16)(L_add(L_shr(L_mult(*seed, 31821), 1), 13849L));
    return (*seed);
}

Word16 exp_mant0(Word32 *L_var)
{
	Word16 exp;

	exp = -norm_l0(*L_var);

	*L_var = L_shl(*L_var, (-exp));
	return exp;
}
