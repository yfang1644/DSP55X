/*-------------------------------------------------------------------*
 *                         PREEMPH.C								 *
 *-------------------------------------------------------------------*
 * Preemphasis: filtering through 1 - g z^-1                         *
 *                                                                   *
 * Preemph2 --> signal is multiplied by 2.                           *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

void Preemph_(
     Word16 x[],         /* (i/o)   : input signal overwritten by the output */
     Word16 mu,          /* (i) Q15 : preemphasis coefficient                */
     Word16 lg           /* (i)     : lenght of filtering                    */
)
{
    Word16 i;
    Word32 L_tmp;

    for (i = lg - 1; i > 0; i--)
    {
        L_tmp = (Word32)x[i] << 16;
        L_tmp = L_msu(L_tmp, x[i - 1], mu);
        x[i] = round16(L_tmp);
    }

    return;
}

void Preemph2(
     Word16 x[],         /* (i/o)   : input signal overwritten by the output */
     Word16 mu,          /* (i) Q15 : preemphasis coefficient                */
     Word16 lg           /* (i)     : lenght of filtering                    */
)
{
    Word16 i;
    Word32 L_tmp;

    for (i = lg - 1; i > 0; i--)
    {
        L_tmp = (Word32)x[i] << 16;
        L_tmp = L_msu(L_tmp, x[i - 1], mu);
        L_tmp = L_shl(L_tmp, 1);
        x[i] = round16(L_tmp);
    }

    x[0] = shl(x[0], 1);

    return;
}
