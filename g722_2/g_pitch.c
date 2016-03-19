/*-------------------------------------------------------------------*
 *                         G_PITCH.C								 *
 *-------------------------------------------------------------------*
 * Compute the gain of pitch. Result in Q12                          *
 *  if (gain < 0)  gain =0                                           *
 *  if (gain > 1.2) gain =1.2                                        *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"

Word16 G_pitch(          /* (o) Q14 : Gain of pitch lag saturated to 1.2   */
     Word16 xn[],        /* (i)     : Pitch target.                        */
     Word16 y1[],        /* (i)     : filtered adaptive codebook.          */
     Word16 g_coeff[],   /* : Correlations need for gain quantization.     */
     Word16 L_subfr      /* : Length of subframe.                          */
)
{
    Word16 i;
    Word16 xy, yy, exp_xy, exp_yy, gain;
    Word32 L_tmp;

    /* Compute scalar product <y1[],y1[]> */

    L_tmp = Dot_product12(y1, y1, L_subfr, &exp_yy);
    yy = (Word16)(L_tmp >> 16);

    /* Compute scalar product <xn[],y1[]> */

    L_tmp = Dot_product12(xn, y1, L_subfr, &exp_xy);
    xy = (Word16)(L_tmp >> 16);
    
    g_coeff[0] = yy;
    g_coeff[1] = exp_yy;
    g_coeff[2] = xy;
    g_coeff[3] = exp_xy;

    xy = xy >> 1;                       /* Be sure xy < yy */
    /* If (xy < 0) gain = 0 */
    if (xy <= 0)
        return ((Word16) 0);

    /* compute gain = xy/yy */

    gain = div_s(xy, yy);

    i = add(exp_xy, 1 - 1);                /* -1 -> gain in Q14 */
    i = sub(i, exp_yy);

    gain = shl(gain, i);                   /* saturation can occur here */

    /* if (gain > 1.2) gain = 1.2  in Q14 */
    if (gain > 19661)
    {
        gain = 19661;
    }
    return (gain);
}
