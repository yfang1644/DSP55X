/*-------------------------------------------------------------------*
 *                         VOICEFAC.C								 *
 *-------------------------------------------------------------------*
 * Find the voicing factor (1=voice to -1=unvoiced).				 *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"

Word16 voice_factor(              /* (o) Q15   : factor (-1=unvoiced to 1=voiced) */
     Word16 exc[],                /* (i) Q_exc : pitch excitation            */
     Word16 Q_exc,                /* (i)       : exc format                  */
     Word16 gain_pit,             /* (i) Q14   : gain of pitch               */
     Word16 code[],               /* (i) Q9    : Fixed codebook excitation   */
     Word16 gain_code,            /* (i) Q0    : gain of code                */
     Word16 L_subfr               /* (i)       : subframe length             */
)
{
    Word16 i, tmp, exp, ener1, exp1, ener2, exp2;
    Word32 L_tmp;

    L_tmp = Dot_product12(exc, exc, L_subfr, &exp1);
    ener1 = (Word16)(L_tmp >> 16);
    exp1 = exp1 - (Q_exc * 2);
    L_tmp = L_mult(gain_pit, gain_pit);

    exp = exp_mant0(&L_tmp);

    tmp = (Word16)(L_tmp >> 16);
    ener1 = mult(ener1, tmp);
    exp1 = exp1 + exp - 10;       /* 10 -> gain_pit Q14 to Q9 */

    L_tmp = Dot_product12(code, code, L_subfr, &exp2);
    ener2 = (Word16)(L_tmp >> 16);

    L_tmp = (Word32)gain_code << 16;
    exp = exp_mant0(&L_tmp);

    tmp = (Word16)(L_tmp >> 16);
    tmp = mult(tmp, tmp);
    ener2 = mult(ener2, tmp);
    exp2 = exp2 + (exp * 2);

    i = exp1 - exp2;

    ener1 >>= 1;
    ener2 >>= 1;
    if (i > 0)
    {
        ener2 = ener2 >> i;
    }
    if (i < 0)
    {
        ener1 = ener1 >> (-i);
    }

    tmp = ener1 - ener2;
    ener1 = ener1 + ener2 + 1;

    if (tmp >= 0)
    {
        tmp = div_s(tmp, ener1);
    } else
    {
        tmp = -div_s(-tmp, ener1);
    }

    return (tmp);
}

