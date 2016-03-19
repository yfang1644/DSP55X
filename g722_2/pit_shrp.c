/*-----------------------------------------------------------------------*
 *                         PIT_SHRP.C									 *
 *-----------------------------------------------------------------------*
 * Performs Pitch sharpening routine				                     *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

void Pit_shrp_(
     Word16 * x,         /* in/out: impulse response (or algebraic code) */
     Word16 pit_lag,     /* input : pitch lag                            */
     Word16 sharp,       /* input : pitch sharpening factor (Q15)        */
     Word16 L_subfr      /* input : subframe size                        */
)
{
    Word16 i;
    Word16 tmp;

    for (i = pit_lag; i < L_subfr; i++)
    {
        tmp = mult_r(x[i - pit_lag], sharp);
        x[i] = add(x[i], tmp);
    }

    return;
}
