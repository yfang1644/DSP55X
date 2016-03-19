/*-------------------------------------------------------------------*
 *                         SCALE.C									 *
 *-------------------------------------------------------------------*
 * Scale signal to get maximum of dynamic.							 *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

void Scale_sig_(
     Word16 x[],       /* (i/o) : signal to scale               */
     Word16 lg,        /* (i)   : size of x[]                   */
     Word16 exp        /* (i)   : exponent: x = round(x << exp) */
)
{
    Word16 i;
    Word32 L_tmp;

    for (i = 0; i < lg; i++)
    {
        L_tmp = (Word32)x[i] << 16;
        L_tmp = L_shl(L_tmp, exp);         /* saturation can occur here */
        x[i] = round16(L_tmp);
    }

    return;
}
