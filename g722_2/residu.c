/*-----------------------------------------------------------------------*
 *                         RESIDU.C										 *
 *-----------------------------------------------------------------------*
 * Compute the LPC residual by filtering the input speech through A(z)   *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

void Residu_(
     Word16 a[],         /* (i) Q12 : prediction coefficients                */
     Word16 m,           /* (i)     : order of LP filter                     */
     Word16 x[],         /* (i)     : speech (values x[-m..-1] are needed    */
     Word16 y[],         /* (o) x2  : residual signal                        */
     Word16 lg           /* (i)     : size of filtering                      */
)
{
    Word16 i, j;
    Word32 s;

    for (i = 0; i < lg; i++)
    {
        s = 0L;
        for (j = 0; j <= m; j++)
        {
            s = L_mac(s, a[j], x[i - j]);
        }
        s = L_shl(s, 3 + 1);
        y[i] = round16(s);
    }

    return;
}
