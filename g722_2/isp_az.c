/*-----------------------------------------------------------------------*
 *                         ISP_AZ.C										 *
 *-----------------------------------------------------------------------*
 * Compute the LPC coefficients from isp (order=M)						 *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"

#define NC (M/2)
#define NC16k (M16k/2)

/* local function */

/*-----------------------------------------------------------*
 * procedure Get_isp_pol:                                    *
 *           ~~~~~~~~~~~                                     *
 *   Find the polynomial F1(z) or F2(z) from the ISPs.       *
 * This is performed by expanding the product polynomials:   *
 *                                                           *
 * F1(z) =   product   ( 1 - 2 isp_i z^-1 + z^-2 )           *
 *         i=0,2,4,6,8                                       *
 * F2(z) =   product   ( 1 - 2 isp_i z^-1 + z^-2 )           *
 *         i=1,3,5,7                                         *
 *                                                           *
 * where isp_i are the ISPs in the cosine domain.            *
 *-----------------------------------------------------------*
 *                                                           *
 * Parameters:                                               *
 *  isp[]   : isp vector (cosine domaine)         in Q15     *
 *  f[]     : the coefficients of F1 or F2        in Q23     *
 *  n       : == NC for F1(z); == NC-1 for F2(z)             *
 *                                                           *
 *    scale :  -256  Get_isp_pol()  (original)               *
 *          :  -64   Get_isp_pol_16kHz()                     *
 *-----------------------------------------------------------*/

void Get_isp_pol(Word16 * isp, Word32 * f, Word16 n, Word16 scale)
{
    Word16 i, j;
    Word32 t0;

    /* All computation in Q23 */

    *f++ = L_mult(16384, scale);             /* f[0] = 1.0;        in Q23  */
    *f++ = L_mult(isp[0], -scale);           /* f[1] = -2.0*isp[0] in Q23  */

    for (i = 2; i <= n; i++)
    {
        isp += 2;                              /* Advance isp pointer        */
        *f = f[-2];

        for (j = 1; j < i; j++, f--)
        {
            t0 = Mpy_32x16(f[-1], *isp);  /* t0 = f[-1] * isp    */
            t0 = L_shl(t0, 1);
            *f = L_sub(*f, t0);            /* *f -= t0            */
            *f = L_add(*f, f[-2]);         /* *f += f[-2]         */
        }
        *f = L_msu(*f, *isp, scale);          /* *f -= isp<<8        */
        f += i;                            /* Advance f pointer   */
    }
    return;
}


void Isp_Az(
     Word16 isp[],          /* (i) Q15 : Immittance spectral pairs            */
     Word16 a[],            /* (o) Q12 : predictor coefficients (order = M)   */
     Word16 m,
     Word16 adaptive_scaling  /* (i) 0   : adaptive scaling disabled */
                              /*     1   : adaptive scaling enabled  */
)
{
    Word16 i, j;
    Word32 f1[NC16k + 1], f2[NC16k];
    Word16 nc, nc_1, m1;
    Word32 t0;
    Word16 q, q_sug;
    Word32 tmax;
    Word32 bit;
    Word16 exp;

    nc = m >> 1;
    nc_1 = nc - 1;
    m1 = m - 1;
    if (nc > 8)
    {
        Get_isp_pol(&isp[0], f1, nc, 64);
        for (i = 0; i <= nc; i++)
        {
            f1[i] = L_shl(f1[i], 2);
        }
        Get_isp_pol(&isp[1], f2, nc_1, 64);
        for (i = 0; i <= nc_1; i++)
        {
            f2[i] = L_shl(f2[i], 2);
        }
    }
    else
    {
        Get_isp_pol(&isp[0], f1, nc, 256);
        Get_isp_pol(&isp[1], f2, nc_1, 256);
    }

    /*-----------------------------------------------------*
     *  Multiply F2(z) by (1 - z^-2)                       *
     *-----------------------------------------------------*/

    for (i = nc_1; i > 1; i--)
    {
        f2[i] = L_sub(f2[i], f2[i - 2]);     /* f2[i] -= f2[i-2]; */
    }

    /*----------------------------------------------------------*
     *  Scale F1(z) by (1+isp[m-1])  and  F2(z) by (1-isp[m-1]) *
     *----------------------------------------------------------*/

    for (i = 0; i < nc; i++)
    {
        /* f1[i] *= (1.0 + isp[M-1]); */
        t0 = Mpy_32x16(f1[i], isp[m1]);
        f1[i] = L_add(f1[i], t0);

        /* f2[i] *= (1.0 - isp[M-1]); */
        t0 = Mpy_32x16(f2[i], isp[m1]);
        f2[i] = L_sub(f2[i], t0);
    }

    /*-----------------------------------------------------*
     *  A(z) = (F1(z)+F2(z))/2                             *
     *  F1(z) is symmetric and F2(z) is antisymmetric      *
     *-----------------------------------------------------*/

    /* a[0] = 1.0; */
    a[0] = 4096;
    tmax = 1;
    bit = 1L << 11;
    for (i = 1, j = m1; i < nc; i++, j--)
    {
        /* a[i] = 0.5*(f1[i] + f2[i]); */

        t0 = L_add(f1[i], f2[i]);          /* f1[i] + f2[i]             */
        tmax |= L_abs(t0);
        t0 = t0 + bit;
        a[i] = (Word16)((t0 >> 12)); /* from Q23 to Q12 and * 0.5 */

        /* a[j] = 0.5*(f1[i] - f2[i]); */

        t0 = L_sub(f1[i], f2[i]);          /* f1[i] - f2[i]             */
        tmax |= L_abs(t0);
        t0 = t0 + bit;
        a[j] = (Word16)((t0 >> 12)); /* from Q23 to Q12 and * 0.5 */
    }

    /* rescale data if overflow has occured and reprocess the loop */

    if (adaptive_scaling == 1)
    {
        exp = norm_l0(tmax);
        q = 4 - exp;               /* adaptive scaling enabled */
    }
    else
        q = 0;                           /* adaptive scaling disabled */

    if (q > 0)
    {
        q_sug = add(12, q);
        bit = 1L << (q_sug - 1);
        for (i = 1, j = m1; i < nc; i++, j--)
        {
          /* a[i] = 0.5*(f1[i] + f2[i]); */

            t0 = L_add(f1[i], f2[i]);          /* f1[i] + f2[i]             */
            t0 = t0 + bit;
            a[i] = (Word16)((t0 >> q_sug)); /* from Q23 to Q12 and * 0.5 */

          /* a[j] = 0.5*(f1[i] - f2[i]); */

            t0 = L_sub(f1[i], f2[i]);          /* f1[i] - f2[i]             */
            t0 = t0 + bit;
            a[j] = (Word16)((t0 >> q_sug)); /* from Q23 to Q12 and * 0.5 */
        }
        a[0] = shr(a[0], q);
    }
    else
    {
        q_sug = 12;
        bit   = 1L << 11;
        q     = 0;
    }

    /* a[NC] = 0.5*f1[NC]*(1.0 + isp[M-1]); */
    t0 = Mpy_32x16(f1[nc], isp[m1]);
    t0 = L_add(t0, f1[nc]);
    t0 = t0 + bit;
    a[nc] = (Word16)((t0 >> q_sug));    /* from Q23 to Q12 and * 0.5 */
    /* a[m] = isp[m-1]; */

    q = q + 2;
    j = isp[m1] >> q;
    a[m] = (j + 1) >> 1;           /* from Q15 to Q12 , with rounding    */

    return;
}
