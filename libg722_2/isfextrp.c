/*-----------------------------------------------------------------------*
 *                         ISFEXTRP.C									 *
 *-----------------------------------------------------------------------*
 *	Conversion of 16th-order 12.8kHz ISF vector 						 *
 *	into 20th-order 16kHz ISF vector									 *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "acelp.h"

#define INV_LENGTH 2731                    /* 1/12 */

void Isf_Extrapolation(Word16 HfIsf[])
{
    Word16 IsfDiff[M - 2];
    Word32 IsfCorr[3];
    Word32 L_tmp;
    Word16 coeff, mean, tmp, tmp2;
    Word16 exp, exp2;
    Word16 i, MaxCorr;

    HfIsf[M16k - 1] = HfIsf[M - 1];

    /* Difference vector */
    tmp = 0;
    for (i = 0; i < (M - 2); i++)
    {
        IsfDiff[i] = sub(HfIsf[i + 1], HfIsf[i]);
        if (IsfDiff[i] > tmp)
        {
            tmp = IsfDiff[i];
        }
    }

    /* Mean of difference vector */
    L_tmp = 0;
    for (i = 2; i < (M - 2); i++)
        L_tmp = L_mac(L_tmp, IsfDiff[i], INV_LENGTH);

//    exp = -exp_mant0(&L_tmp);
//    mean = round16(L_tmp);
    mean = round16(L_tmp);
    exp = norm_s0(tmp);
    mean = shl(mean, exp);

    for (i = 0; i < (M - 2); i++)
    {
        IsfDiff[i] = shl(IsfDiff[i], exp);
    }

    IsfCorr[0] = 0;
    IsfCorr[1] = 0;
    IsfCorr[2] = 0;

    for (i = 7; i < (M - 2); i++)
    {
        tmp = sub(IsfDiff[i], mean);
        tmp2 = sub(IsfDiff[i - 2], mean);
        L_tmp = L_mult(tmp, tmp2);
        L_tmp = Mpy_32x32(L_tmp, L_tmp);
        IsfCorr[0] = L_add(IsfCorr[0], L_tmp);

        tmp2 = sub(IsfDiff[i - 3], mean);
        L_tmp = L_mult(tmp, tmp2);
        L_tmp = Mpy_32x32(L_tmp, L_tmp);
        IsfCorr[1] = L_add(IsfCorr[1], L_tmp);

        tmp2 = sub(IsfDiff[i - 4], mean);
        L_tmp = L_mult(tmp, tmp2);
        L_tmp = Mpy_32x32(L_tmp, L_tmp);
        IsfCorr[2] = L_add(IsfCorr[2], L_tmp);
    }

    if (IsfCorr[0] > IsfCorr[1])
    {
        MaxCorr = 0;
    } else
    {
        MaxCorr = 1;
    }

    if (IsfCorr[2] > IsfCorr[MaxCorr])
        MaxCorr = 2;

    MaxCorr++;                  /* Maximum correlation of difference vector */

    for (i = M - 1; i < (M16k - 1); i++)
    {
        tmp = sub(HfIsf[i - 1 - MaxCorr], HfIsf[i - 2 - MaxCorr]);
        HfIsf[i] = add(HfIsf[i - 1], tmp);
    }

    /* tmp=7965+(HfIsf[2]-HfIsf[3]-HfIsf[4])/6; */
    tmp = add(HfIsf[4], HfIsf[3]);
    tmp = sub(HfIsf[2], tmp);
    tmp = mult(tmp, 5461);
    tmp = add(tmp, 20390);

    if (tmp > 19456)
    {                 /* Maximum value of ISF should be at most 7600 Hz */
        tmp = 19456;
    }
    tmp = sub(tmp, HfIsf[M - 2]);
    tmp2 = sub(HfIsf[M16k - 2], HfIsf[M - 2]);

    exp2 = norm_s0(tmp2);
    exp = norm_s0(tmp);
    exp = exp - 1;
    tmp = shl(tmp, exp);
    tmp2 = shl(tmp2, exp2);
    coeff = div_s(tmp, tmp2);  /* Coefficient for stretching the ISF vector */
    exp = sub(exp2, exp);

    for (i = M - 1; i < (M16k - 1); i++)
    {
        tmp = mult(sub(HfIsf[i], HfIsf[i - 1]), coeff);
        IsfDiff[i - (M - 1)] = shl(tmp, exp);
    }

    for (i = M; i < (M16k - 1); i++)
    {
        /* The difference between ISF(n) and ISF(n-2) should be at least 500 Hz */
        tmp = sub(add(IsfDiff[i - (M - 1)], IsfDiff[i - M]), 1280);
        if (tmp < 0)
        {
            if (IsfDiff[i - (M - 1)] > IsfDiff[i - M])
            {
                IsfDiff[i - M] = sub(1280, IsfDiff[i - (M - 1)]);
            } else
            {
                IsfDiff[i - (M - 1)] = sub(1280, IsfDiff[i - M]);
            }
        }
    }

    for (i = M - 1; i < (M16k - 1); i++)
    {
        HfIsf[i] = add(HfIsf[i - 1], IsfDiff[i - (M - 1)]);
    }

    for (i = 0; i < (M16k - 1); i++)
    {
        HfIsf[i] = mult(HfIsf[i], 26214);  /* Scale the ISF vector correctly for 16000 kHz */
    }

    Isf_isp(HfIsf, HfIsf, M16k);

    return;
}
