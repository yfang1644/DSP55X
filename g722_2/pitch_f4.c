/*-----------------------------------------------------------------------*
 *                         PITCH_F4.C									 *
 *-----------------------------------------------------------------------*
 * Find the closed loop pitch period with 1/4 subsample resolution.		 *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "acelp.h"
#include "cnst.h"

#define UP_SAMP      4
#define L_INTERPOL1  4

/*--------------------------------------------------------------------------*
 * Function Norm_Corr()                                                     *
 * ~~~~~~~~~~~~~~~~~~~~                                                     *
 * Find the normalized correlation between the target vector and the        *
 * filtered past excitation.                                                *
 * (correlation between target and filtered excitation divided by the       *
 *  square root of energy of target and filtered excitation).               *
 *--------------------------------------------------------------------------*/

void Norm_Corr(
     Word16 exc[],                /* (i)     : excitation buffer             */
     Word16 xn[],                 /* (i)     : target vector                 */
     Word16 h[],                  /* (i) Q15 : impulse response of synth/wgt filters */
     Word16 L_subfr,              /* (i)     : Length of subframe            */
     Word16 t_min,                /* (i)     : minimum value of pitch lag.   */
     Word16 t_max,                /* (i)     : maximum value of pitch lag.   */
     Word16 corr_norm[])          /* (o) Q15 : normalized correlation        */
{
    Word16 i, k, t;
    Word16 corr, exp_corr, norm, exp_norm, exp, scale;
    Word16 excf[L_SUBFR];
    Word32 L_tmp;

    /* compute the filtered excitation for the first delay t_min */

    k = -t_min;
    Convolve(&exc[k], h, excf, L_subfr);

    /* Compute rounded down 1/sqrt(energy of xn[]) */

    L_tmp = Dot_product12(xn, xn, L_subfr, &exp);

    scale = exp >> 1;                /* energy of xn[] x 2 + rounded up     */
    scale++;                         /* (1<<scale) < 1/sqrt(energy rounded) */

    /* loop for every possible period */
    for (t = t_min; t <= t_max; t++)
    {
        /* Compute correlation between xn[] and excf[] */

        L_tmp = Dot_product12(xn, excf, L_subfr, &exp_corr);
        corr = (Word16)(L_tmp >> 16);

        /* Compute 1/sqrt(energy of excf[]) */

        L_tmp = Dot_product12(excf, excf, L_subfr, &exp_norm);

        L_tmp = Isqrt_n(L_tmp, &exp_norm);
        norm = (Word16)(L_tmp >> 16);

        /* Normalize correlation = correlation * (1/sqrt(energy)) */

        L_tmp = L_mult(corr, norm);
        L_tmp = L_shl(L_tmp, sub(sub(exp_corr, exp_norm), scale));

        corr_norm[t] = round16(L_tmp);

        /* modify the filtered excitation excf[] for the next iteration */

        if (t != t_max)
        {
            k--;
            for (i = (L_subfr - 1); i > 0; i--)
            {
                /* saturation can occur in add() */
                excf[i] = add(mult(exc[k], h[i]), excf[i - 1]);
            }
            excf[0] = mult(exc[k], h[0]);
        }
    }

    return;
}

/*--------------------------------------------------------------------------*
 * Procedure Interpol_4()                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~                                                   *
 * For interpolating the normalized correlation with 1/4 resolution.        *
 *--------------------------------------------------------------------------*/

/* 1/4 resolution interpolation filter (-3 dB at 0.791*fs/2) in Q14 */

Word16 inter4_1[UP_SAMP * 2 * L_INTERPOL1] =
{
    -12, -26, 32, 206,
    420, 455, 73, -766,
    -1732, -2142, -1242, 1376,
    5429, 9910, 13418, 14746,
    13418, 9910, 5429, 1376,
    -1242, -2142, -1732, -766,
    73, 455, 420, 206,
    32, -26, -12, 0
};

/*** Coefficients in floating point
static float inter4_1[UP_SAMP*L_INTERPOL1+1] = {
   0.900000,
   0.818959,  0.604850,  0.331379,  0.083958,
  -0.075795, -0.130717, -0.105685, -0.046774,
   0.004467,  0.027789,  0.025642,  0.012571,
   0.001927, -0.001571, -0.000753,  0.000000};
***/

Word16 Interpol_4(                  /* (o)  : interpolated value  */
     Word16 * x,                           /* (i)  : input vector        */
     Word16 frac                           /* (i)  : fraction (-4..+3)   */
)
{
    Word16 i, k, sum;
    Word32 L_sum;

    if (frac < 0)
    {
        frac = frac + UP_SAMP;
        x--;
    }
    x = x - L_INTERPOL1 + 1;

    L_sum = 0L;
    for (i = 0, k = UP_SAMP - 1 - frac; i < 2 * L_INTERPOL1; i++, k += UP_SAMP)
    {
        L_sum = L_mac(L_sum, x[i], inter4_1[k]);
    }

    sum = round16(L_shl(L_sum, 1));

    return (sum);
}

Word16 Pitch_fr4(         /* (o)     : pitch period.                 */
     Word16 exc[],        /* (i)     : excitation buffer             */
     Word16 xn[],         /* (i)     : target vector                 */
     Word16 h[],          /* (i) Q15 : impulse response of synth/wgt filters */
     Word16 t0_min,       /* (i)     : minimum value in the searched range.  */
     Word16 t0_max,       /* (i)     : maximum value in the searched range.  */
     Word16 * pit_frac,   /* (o)     : chosen fraction (0, 1, 2 or 3). */
     Word16 i_subfr,      /* (i)     : indicator for first subframe. */
     Word16 t0_fr2,       /* (i)     : minimum value for resolution 1/2      */
     Word16 t0_fr1,       /* (i)     : minimum value for resolution 1*/
     Word16 L_subfr       /* (i)     : Length of subframe            */
)
{
    Word16 i;
    Word16 t_min, t_max;
    Word16 max, t0, fraction, step, temp;
    Word16 *corr;
    Word16 corr_v[40];                     /* Total length = t0_max-t0_min+1+2*L_inter */

    /* Find interval to compute normalized correlation */

    t_min = sub(t0_min, L_INTERPOL1);
    t_max = add(t0_max, L_INTERPOL1);

    corr = &corr_v[-t_min];

    /* Compute normalized correlation between target and filtered excitation */

    Norm_Corr(exc, xn, h, L_subfr, t_min, t_max, corr);

    /* Find integer pitch */

    max = corr[t0_min];
    t0 = t0_min;

    for (i = t0_min + 1; i <= t0_max; i++)
    {
        if (corr[i] >= max)
        {
            max = corr[i];
            t0 = i;
        }
    }

    /* If first subframe and t0 >= t0_fr1, do not search fractionnal pitch */
    if ((i_subfr == 0) && (t0 >= t0_fr1))
    {
        *pit_frac = 0;
        return (t0);
    }
    /*------------------------------------------------------------------*
     * Search fractionnal pitch with 1/4 subsample resolution.          *
     * Test the fractions around t0 and choose the one which maximizes  *
     * the interpolated normalized correlation.                         *
     *------------------------------------------------------------------*/

    step = 1;               /* 1/4 subsample resolution */
    fraction = -3;
    if (((i_subfr == 0) && (t0 >= t0_fr2)) || (t0_fr2 == PIT_MIN))
    {
        step = 2;           /* 1/2 subsample resolution */
        fraction = -2;
    }
    if (t0 == t0_min)
    {
        fraction = 0;
    }

    max = Interpol_4(&corr[t0], fraction);
    for (i = fraction + step; i <= 3; i += step)
    {
        temp = Interpol_4(&corr[t0], i);

        if (temp > max)
        {
            max = temp;
            fraction = i;
        }
    }

    /* limit the fraction value in the interval [0,1,2,3] */
    if (fraction < 0)
    {
        fraction = fraction + UP_SAMP;
        t0--;
    }
    *pit_frac = fraction;

    return (t0);
}

