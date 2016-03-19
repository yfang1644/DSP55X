/*-------------------------------------------------------------------*
 *                         WB_VAD.C									 *
 *-------------------------------------------------------------------*
 * Weighting of LPC coefficients.									 *
 *   ap[i]  =  a[i] * (gamma ** i)									 *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

void Weight_a_(
     Word16 a[],          /* (i) Q12 : a[m+1]  LPC coefficients             */
     Word16 ap[],         /* (o) Q12 : Spectral expanded LPC coefficients   */
     Word16 gamma,        /* (i) Q15 : Spectral expansion factor.           */
     Word16 m             /* (i)     : LPC order.                           */
)
{
    Word16 i, fac;

    ap[0] = a[0];
    fac = gamma;
    for (i = 1; i < m; i++)
    {
        ap[i] = mult_r(a[i], fac);
        fac = mult_r(fac, gamma);
    }
    ap[m] = mult_r(a[m], fac);

    return;
}
