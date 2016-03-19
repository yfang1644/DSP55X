/*-----------------------------------------------------------------------*
 *                         Az_isp.c                                      *
 *-----------------------------------------------------------------------*
 * Compute the ISPs from  the LPC coefficients  (order=M)                *
 *-----------------------------------------------------------------------*
 *                                                                       *
 * The ISPs are the roots of the two polynomials F1(z) and F2(z)         *
 * defined as                                                            *
 *               F1(z) = A(z) + z^-m A(z^-1)                             *
 *  and          F2(z) = A(z) - z^-m A(z^-1)                             *
 *                                                                       *
 * For a even order m=2n, F1(z) has M/2 conjugate roots on the unit      *
 * circle and F2(z) has M/2-1 conjugate roots on the unit circle in      *
 * addition to two roots at 0 and pi.                                    *
 *                                                                       *
 * For a 16th order LP analysis, F1(z) and F2(z) can be written as       *
 *                                                                       *
 *   F1(z) = (1 + a[M])   PRODUCT  (1 - 2 cos(w_i) z^-1 + z^-2 )         *
 *                        i=0,2,4,6,8,10,12,14                           *
 *                                                                       *
 *   F2(z) = (1 - a[M]) (1 - z^-2) PRODUCT (1 - 2 cos(w_i) z^-1 + z^-2 ) *
 *                                 i=1,3,5,7,9,11,13                     *
 *                                                                       *
 * The ISPs are the M-1 frequencies w_i, i=0...M-2 plus the last         *
 * predictor coefficient a[M].                                           *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"

/*-------------------------------------------------------------*
 *  Table for az_isp()                                         *
 *                                                             *
 * Vector grid[] is in Q15                                     *
 *                                                             *
 * grid[0] = 1.0;                                              *
 * grid[grid_points+1] = -1.0;                                 *
 * for (i = 1; i < grid_points; i++)                           *
 *   grid[i] = cos((6.283185307*i)/(2.0*grid_points));         *
 *                                                             *
 *-------------------------------------------------------------*/

/* Version 101 points */

#define   GRID_POINTS     100

Word16 grid[GRID_POINTS+1] ={
    32767,     32751,     32703,     32622,     32509,     32364,
    32187,     31978,     31738,     31466,     31164,     30830,
    30466,     30072,     29649,     29196,     28714,     28204,
    27666,     27101,     26509,     25891,     25248,     24579,
    23886,     23170,     22431,     21669,     20887,     20083,
    19260,     18418,     17557,     16680,     15786,     14876,
    13951,     13013,     12062,     11099,     10125,      9141,
     8149,      7148,      6140,      5126,      4106,      3083,
     2057,      1029,         0,     -1029,     -2057,     -3083,
    -4106,     -5126,     -6140,     -7148,     -8149,     -9141,
   -10125,    -11099,    -12062,    -13013,    -13951,    -14876,
   -15786,    -16680,    -17557,    -18418,    -19260,    -20083,
   -20887,    -21669,    -22431,    -23170,    -23886,    -24579,
   -25248,    -25891,    -26509,    -27101,    -27666,    -28204,
   -28714,    -29196,    -29649,    -30072,    -30466,    -30830,
   -31164,    -31466,    -31738,    -31978,    -32187,    -32364,
   -32509,    -32622,    -32703,    -32751,    -32760};



#define M   16
#define NC  (M/2)

/* local function */

/*--------------------------------------------------------------------*
 * function  Chebps2:                                                 *
 *           ~~~~~~~                                                  *
 *    Evaluates the Chebishev polynomial series                       *
 *--------------------------------------------------------------------*
 *                                                                    *
 *  The polynomial order is                                           *
 *     n = M/2   (M is the prediction order)                          *
 *  The polynomial is given by                                        *
 *    C(x) = f(0)T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2   *
 * Arguments:                                                         *
 *  x:     input value of evaluation; x = cos(frequency) in Q15       *
 *  f[]:   coefficients of the pol.                      in Q11       *
 *  n:     order of the pol.                                          *
 *                                                                    *
 * The value of C(x) is returned. (Satured to +-1.99 in Q14)          *
 *                                                                    *
 *--------------------------------------------------------------------*/

Word16 Chebps2(Word16 x, Word16 f[], Word16 n)
{
    Word16 i, cheb;
    Word32 t0;
    Word32 b0, b1, b2;

    /* Note: All computation are done in Q24. */

    b2 = L_mult(f[0], 4096);               /* b2 = f[0] in Q24 DPF */

    t0 = Mpy_32x16(b2, x);                 /* t0 = 2.0*x*b2        */
    t0 = L_shl(t0, 1);
    b1 = L_mac(t0, f[1], 4096);            /* + f[1] in Q24        */
                                           /* b1 = 2*x*b2 + f[1]   */
    for (i = 2; i < n; i++)
    {
        t0 = Mpy_32x16(b1, x);
        t0 = L_shl(t0, 1);                 /* t0 = 2.0*x*b1              */
        t0 = L_mac(t0, f[i], 4096);
        b0 = L_sub(t0, b2);                /* b0 = 2.0*x*b1 - b2 + f[i]; */

        b2 = b1;
        b1 = b0;
    }

    t0 = Mpy_32x16(b1, x);                 /* t0 = x*b1;              */
    t0 = L_sub(t0, b2);                    /* t0 = x*b1 - b2          */
    t0 = L_mac(t0, f[n], 2048);            /* t0 = x*b1 - b2 + f[n]/2 */

    t0 = L_shl(t0, 6);                     /* Q24 to Q30 with saturation */

    cheb = (Word16)(t0 >> 16);             /* Result in Q14              */

    return (cheb);
}

void Az_isp(
     Word16 a[],                  /* (i) Q12 : predictor coefficients        */
     Word16 isp[],                /* (o) Q15 : Immittance spectral pairs     */
     Word16 old_isp[]             /* (i)     : old isp[] (in case not found M roots)  */
)
{
    Word16 i, j, nf, ip, order;
    Word16 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
    Word16 x, y, sign, exp;
    Word16 *coef;
    Word16 f1[NC + 1], f2[NC];
    Word32 t0;

    /*-------------------------------------------------------------*
     * find the sum and diff polynomials F1(z) and F2(z)           *
     *      F1(z) = [A(z) + z^M A(z^-1)]                           *
     *      F2(z) = [A(z) - z^M A(z^-1)]/(1-z^-2)                  *
     *                                                             *
     * for (i=0; i<NC; i++)                                        *
     * {                                                           *
     *   f1[i] = a[i] + a[M-i];                                    *
     *   f2[i] = a[i] - a[M-i];                                    *
     * }                                                           *
     * f1[NC] = 2.0*a[NC];                                         *
     *                                                             *
     * for (i=2; i<NC; i++)            Divide by (1-z^-2)          *
     *   f2[i] += f2[i-2];                                         *
     *-------------------------------------------------------------*/

    for (i = 0; i < NC; i++)
    {
        t0 = L_mult(a[i], 16384);
        f1[i] = round16(L_mac(t0, a[M - i], 16384));    /* =(a[i]+a[M-i])/2 */
        f2[i] = round16(L_msu(t0, a[M - i], 16384));    /* =(a[i]-a[M-i])/2 */
    }
    f1[NC] = a[NC];

    for (i = 2; i < NC; i++)               /* Divide by (1-z^-2) */
        f2[i] = add(f2[i], f2[i - 2]);

    /*---------------------------------------------------------------------*
     * Find the ISPs (roots of F1(z) and F2(z) ) using the                 *
     * Chebyshev polynomial evaluation.                                    *
     * The roots of F1(z) and F2(z) are alternatively searched.            *
     * We start by finding the first root of F1(z) then we switch          *
     * to F2(z) then back to F1(z) and so on until all roots are found.    *
     *                                                                     *
     *  - Evaluate Chebyshev pol. at grid points and check for sign change.*
     *  - If sign change track the root by subdividing the interval        *
     *    2 times and ckecking sign change.                                *
     *---------------------------------------------------------------------*/

    nf = 0;                            /* number of found frequencies */
    ip = 0;                            /* indicator for f1 or f2      */

    coef = f1;
    order = NC;

    xlow = grid[0];
    ylow = Chebps2(xlow, coef, order);

    j = 0;
    while ((nf < M - 1) && (j < GRID_POINTS))
    {
        j = j + 1;
        xhigh = xlow;
        yhigh = ylow;
        xlow = grid[j];
        ylow = Chebps2(xlow, coef, order);

        if (L_mult(ylow, yhigh) <= (Word32) 0)
        {
            /* divide 2 times the interval */

            for (i = 0; i < 2; i++)
            {
                xmid = (xlow >> 1) + (xhigh >> 1); /* xmid =(xlow + xhigh)/2 */

                ymid = Chebps2(xmid, coef, order);

                if (L_mult(ylow, ymid) <= (Word32) 0)
                {
                    yhigh = ymid;
                    xhigh = xmid;
                } else
                {
                    ylow = ymid;
                    xlow = xmid;
                }
            }

            /*-------------------------------------------------------------*
             * Linear interpolation                                        *
             *    xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);            *
             *-------------------------------------------------------------*/

            x = xhigh - xlow;
            y = yhigh - ylow;

            if (y == 0)
            {
                xint = xlow;
            } else
            {
                sign = y;
                y = abs_s(y);
                exp = norm_s0(y);
                y = shl(y, exp);
                y = div_s(0x3fff, y);
                t0 = L_mult(x, y);
                t0 = L_shr(t0, sub(20, exp));
                y = (Word16) (t0);         /* y= (xhigh-xlow)/(yhigh-ylow) in Q11 */

                if (sign < 0)
                    y = -y;

                t0 = L_mult(ylow, y);      /* result in Q26 */
                x = (Word16)(t0 >> 11);             /* result in Q15 */
                xint = sub(xlow, x);   /* xint = xlow - ylow*y */
            }

            isp[nf] = xint;
            xlow = xint;
            nf++;

            if (ip == 0)
            {
                coef = f2;
            } else
            {
                coef = f1;
            }
            ip = 1 - ip;
            order = NC - ip;
            ylow = Chebps2(xlow, coef, order);
        }
    }

    /* Check if M-1 roots found */

    if (nf < M - 1)
    {
        for (i = 0; i < M; i++)
        {
            isp[i] = old_isp[i];
        }
    } else
    {
        isp[M - 1] = shl(a[M], 3);       /* From Q12 to Q15 with saturation */
    }

    return;
}

