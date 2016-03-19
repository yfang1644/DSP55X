/*-------------------------------------------------------------------*
 *                         LP_DEC2.C								 *
 *-------------------------------------------------------------------*
 * Decimate a vector by 2 with 2nd order fir filter.                 *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "cnst.h"

#define L_FIR  5
#define L_MEM  (L_FIR-2)

/* static float h_fir[L_FIR] = {0.13, 0.23, 0.28, 0.23, 0.13}; */
/* fixed-point: sum of coef = 32767 to avoid overflow on DC */
Word16 h_fir[L_FIR] = {4260, 7536, 9175, 7536, 4260};


void LP_Decim2(
     Word16 x[],             /* in/out: signal to process         */
     Word16 l,               /* input : size of filtering         */
     Word16 mem[]            /* in/out: memory (size=3)           */
)
{
    Word16 *p_x, x_buf[L_FRAME + L_MEM];
    Word16 i, j;
    Word32 L_tmp;

    /* copy initial filter states into buffer */

    p_x = x_buf;
    for (i = 0; i < L_MEM; i++)
    {
        *p_x++ = mem[i];
        mem[i] = x[l - L_MEM + i];
    }
    for (i = 0; i < l; i++)
    {
        *p_x++ = x[i];
    }

    for (i = 0, j = 0; i < l; i += 2, j++)
    {
        L_tmp = Dot_product(&x_buf[i], h_fir, L_FIR);

        x[j] = round16(L_tmp);
    }

    return;
}
