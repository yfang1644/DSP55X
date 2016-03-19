/*-------------------------------------------------------------------*
 *                         cor_h_x.C                                 *
 *-------------------------------------------------------------------*
 * Compute correlation between target "x[]" and "h[]".               *
 * Designed for codebook search (24 pulses, 4 tracks, 4 pulses per   *
 *    track, 16 positions in each track) to avoid saturation.        *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "cnst.h"

#define NB_TRACK  4
#define STEP      4

void cor_h_x(
     Word16 h[],        /* (i) Q12 : impulse response of weighted synthesis filter */
     Word16 x[],        /* (i) Q0  : target vector                           */
     Word16 dn[]        /* (o) <12bit : correlation between target and h[]   */
)
{
    Word16 i, j, k;
    Word32 L_tmp, y32[L_SUBFR], L_max, L_tot;

    /* first keep the result on 32 bits and find absolute maximum */

    L_tot = 1L;

    for (k = 0; k < NB_TRACK; k++)
    {
        L_max = 0;
        for (i = k; i < L_SUBFR; i += STEP)
        {
            L_tmp = Dot_product(h, &x[i], L_SUBFR - i);

            y32[i] = L_tmp;

            L_tmp = L_abs(L_tmp);
            if (L_tmp > L_max)
            {
                L_max = L_tmp;
            }
        }
        /* tot += 3*max / 8 */
        L_max = L_max >> 2;
        L_tot = L_add(L_tot, L_max);            /* +max/4 */
        L_tot = L_add(L_tot, (L_max >> 1));     /* +max/8 */
    }

    /* Find the number of right shifts to do on y32[] so that    */
    /* 6.0 x sumation of max of dn[] in each track not saturate. */
    j = norm_l0(L_tot);
    j = j - 4;             /* 4 -> 16 x tot */

    for (i = 0; i < L_SUBFR; i++)
    {
        dn[i] = round16(L_shl(y32[i], j));
    }

    return;
}
