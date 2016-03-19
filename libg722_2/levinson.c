/*---------------------------------------------------------------------------*
 *                         LEVINSON.C										 *
 *---------------------------------------------------------------------------*
 *                                                                           *
 *      LEVINSON-DURBIN algorithm in double precision                        *
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        *
 *                                                                           *
 * Algorithm                                                                 *
 *                                                                           *
 *       R[i]    autocorrelations.                                           *
 *       A[i]    filter coefficients.                                        *
 *       K       reflection coefficients.                                    *
 *       Alpha   prediction gain.                                            *
 *                                                                           *
 *       Initialization:                                                     *
 *               A[0] = 1                                                    *
 *               K    = -R[1]/R[0]                                           *
 *               A[1] = K                                                    *
 *               Alpha = R[0] * (1-K**2]                                     *
 *                                                                           *
 *       Do for  i = 2 to M                                                  *
 *                                                                           *
 *            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]                      *
 *                                                                           *
 *            K = -S / Alpha                                                 *
 *                                                                           *
 *            An[j] = A[j] + K*A[i-j]   for j=1 to i-1                       *
 *                                      where   An[i] = new A[i]             *
 *            An[i]=K                                                        *
 *                                                                           *
 *            Alpha=Alpha * (1-K**2)                                         *
 *                                                                           *
 *       END                                                                 *
 *                                                                           *
 * Remarks on the dynamics of the calculations.                              *
 *                                                                           *
 *       The numbers used are in double precision in the following format :  *
 *       A = AH <<16 + AL<<1.  AH and AL are 16 bit signed integers.         *
 *       Since the LSB's also contain a sign bit, this format does not       *
 *       correspond to standard 32 bit integers.  We use this format since   *
 *       it allows fast execution of multiplications and divisions.          *
 *                                                                           *
 *       "DPF" will refer to this special format in the following text.      *
 *       See oper_32b.c                                                      *
 *                                                                           *
 *       The R[i] were normalized in routine AUTO (hence, R[i] < 1.0).       *
 *       The K[i] and Alpha are theoretically < 1.0.                         *
 *       The A[i], for a sampling frequency of 8 kHz, are in practice        *
 *       always inferior to 16.0.                                            *
 *                                                                           *
 *       These characteristics allow straigthforward fixed-point             *
 *       implementation.  We choose to represent the parameters as           *
 *       follows :                                                           *
 *                                                                           *
 *               R[i]    Q31   +- .99..                                      *
 *               K[i]    Q31   +- .99..                                      *
 *               Alpha   Normalized -> mantissa in Q31 plus exponent         *
 *               A[i]    Q27   +- 15.999..                                   *
 *                                                                           *
 *       The additions are performed in 32 bit.  For the summation used      *
 *       to calculate the K[i], we multiply numbers in Q31 by numbers        *
 *       in Q27, with the result of the multiplications in Q27,              *
 *       resulting in a dynamic of +- 16.  This is sufficient to avoid       *
 *       overflow, since the final result of the summation is                *
 *       necessarily < 1.0 as both the K[i] and Alpha are                    *
 *       theoretically < 1.0.                                                *
 *___________________________________________________________________________*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"

#define M   16
#define NC  (M/2)

void Levinson(
     Word32 Rh[],      /* (i)     : Rh[M+1] Vector of autocorrelations (msb) */
     Word16 A[],       /* (o) Q12 : A[M]    LPC coefficients  (m = 16)       */
     Word16 rc[],      /* (o) Q15 : rc[M]   Reflection coefficients.         */
     Word16 * mem      /* (i/o)   :static memory (18 words)                  */
)
{
    Word16 i, j;
    Word32 Kh;                 /* reflection coefficient; hi and lo           */
    Word32 alp;                /* Prediction gain;         */
    Word16 alp_exp;            /* and exponent              */
    Word32 Ah[M + 1];          /* LPC coef. in double prec.    */
    Word32 Anh[M + 1];       /* LPC coef.for next iteration in double prec. */
    Word32 t0, t1, t2;       /* temporary variable                          */
    Word16 *old_A, *old_rc;

    /* Last A(z) for case of unstable filter */

    old_A = mem;
    old_rc = mem + M;

    /* K = A[1] = -R[1] / R[0] */

    t1 = Rh[1];                            /* R[1] in Q31      */
    t2 = L_abs(t1);                        /* abs R[1]         */
    Kh = Div_32_32(t2, Rh[0]);             /* R[1]/R[0] in Q31 */
    if (t1 > 0)
        Kh = -Kh;                          /* -R[1]/R[0]       */
    rc[0] = (Word16)(Kh > 16);
    Ah[1] = Kh >> 4;                       /* A[1] in Q27      */

    /* Alpha = R[0] * (1-K**2) */

    t0 = Mpy_32x32(Kh, Kh);                /* K*K      in Q31 */
    t0 = L_abs(t0);                        /* Some case <0 !! */
    t0 = ~t0;
    t0 &= 0x7fffffffL;                     /* 1 - K*K  in Q31 */
    alp = Mpy_32x32(Rh[0], t0);            /* Alpha in Q31    */

    /* Normalize Alpha */

    alp_exp = exp_mant0(&alp);

    /*--------------------------------------*
     * ITERATIONS  I=2 to M                 *
     *--------------------------------------*/

    for (i = 2; i <= M; i++)
    {
        /* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */

        t0 = 0;
        for (j = 1; j < i; j++)
            t0 = L_add(t0, Mpy_32x32(Rh[j], Ah[i - j]));

        t0 = L_shl(t0, 4);                /* result in Q27 -> convert to Q31 */
                                          /* No overflow possible            */
        t0 = L_add(t0, Rh[i]);            /* add R[i] in Q31                 */

        /* K = -t0 / Alpha */

        t1 = L_abs(t0);
        t2 = Div_32_32(t1, alp);          /* abs(t0)/Alpha                   */
        if (t0 > 0)
            t2 = -t2;                     /* K =-t0/Alpha                    */
        Kh = L_shl(t2, -alp_exp);          /* denormalize; compare to Alpha   */
        rc[i - 1] = (Word16)(Kh >> 16);

        /* Test for unstable filter. If unstable keep old A(z) */

        if (abs_s(rc[i - 1]) > 32750)
        {
            A[0] = 4096;             /* Ai[0] not stored (always 1.0) */
            for (j = 0; j < M; j++)
            {
                A[j + 1] = old_A[j];
            }
            rc[0] = old_rc[0];       /* only two rc coefficients are needed */
            rc[1] = old_rc[1];
            return;
        }
        /*------------------------------------------*
         *  Compute new LPC coeff. -> An[i]         *
         *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
         *  An[i]= K                                *
         *------------------------------------------*/

        for (j = 1; j < i; j++)
        {
            t0 = Mpy_32x32(Kh, Ah[i - j]);
            Anh[j] = L_add(t0, Ah[j]);
        }
        Anh[i] = Kh >> 4;                /* t2 = K in Q31 ->convert to Q27  */

        /* Alpha = Alpha * (1-K**2) */

        t0 = Mpy_32x32(Kh, Kh);            /* K*K      in Q31 */
        t0 = L_abs(t0);                    /* Some case <0 !! */
        t0 = ~t0;
        t0 &= 0x7fffffffL;                 /* 1 - K*K  in Q31 */
        alp = Mpy_32x32(alp, t0);          /* Alpha in Q31    */

        /* Normalize Alpha */

        j = exp_mant0(&alp);
        alp_exp = alp_exp + j;         /* Add normalization to alp_exp */

        /* A[j] = An[j] */

        for (j = 1; j <= i; j++)
        {
            Ah[j] = Anh[j];
        }
    }

    /* Truncate A[i] in Q27 to Q12 with rounding */

    A[0] = 4096;
    for (i = 1; i <= M; i++)
    {
        A[i] = round16(L_shl(Ah[i], 1));
        old_A[i - 1] = A[i];
    }
    old_rc[0] = rc[0];
    old_rc[1] = rc[1];

    return;
}
