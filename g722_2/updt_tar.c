/*-------------------------------------------------------------------*
 *                         UPD_TAR.C								 *
 *-------------------------------------------------------------------*
 * Update the target vector for codebook search.				     *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

void Updt_tar_(
     Word16 * x,         /* (i) Q0  : old target (for pitch search)     */
     Word16 * x2,        /* (o) Q0  : new target (for codebook search)  */
     Word16 * y,         /* (i) Q0  : filtered adaptive codebook vector */
     Word16 gain,        /* (i) Q14 : adaptive codebook gain            */
     Word16 L            /* (i)     : subframe size                     */
)
{
    Word16 i;//, tmp;
    Word32 L_tmp;

    for (i = 0; i < L; i++)
    {
        L_tmp = L_mult(x[i], 16384);
        L_tmp = L_msu(L_tmp, y[i], gain);
        L_tmp = L_shl(L_tmp, 1);			//OVERflowed !!!
        x2[i] = (Word16)(L_tmp >> 16);
    }

    return;
}
