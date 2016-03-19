/*-------------------------------------------------------------------*
 *                         DEEMPH.C									 *
 *-------------------------------------------------------------------*
 * Deemphasis: filtering through 1/(1-mu z^-1)				         *
 *																	 *
 * Deemph2   --> signal is divided by 2.							 *
 * Deemph_32 --> for 32 bits signal.								 *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"

void Deemph_(
     Word16 x[],         /* (i/o)   : input signal overwritten by the output */
     Word16 mu,          /* (i) Q15 : deemphasis factor                      */
     Word16 L,           /* (i)     : vector size                            */
     Word16 * mem        /* (i/o)   : memory (y[-1])                         */
)
{
    Word16 i;
    Word32 L_tmp;

    L_tmp = (Word32)x[0] << 16;
    L_tmp = L_mac(L_tmp, *mem, mu);
    x[0] = round16(L_tmp);

    for (i = 1; i < L; i++)
    {
        L_tmp = (Word32)x[i] << 16;
        L_tmp = L_mac(L_tmp, x[i - 1], mu);
        x[i] = round16(L_tmp);
    }

    *mem = x[L - 1];

    return;
}

void Deemph2_(
     Word16 x[],         /* (i/o)   : input signal overwritten by the output */
     Word16 mu,          /* (i) Q15 : deemphasis factor                      */
     Word16 L,           /* (i)     : vector size                            */
     Word16 * mem        /* (i/o)   : memory (y[-1])                         */
)
{
    Word16 i;
    Word32 L_tmp;

    /* saturation can occur in L_mac() */

    L_tmp = L_mult(x[0], 16384);
    L_tmp = L_mac(L_tmp, *mem, mu);
    x[0] = round16(L_tmp);

    for (i = 1; i < L; i++)
    {
        L_tmp = L_mult(x[i], 16384);
        L_tmp = L_mac(L_tmp, x[i - 1], mu);
        x[i] = round16(L_tmp);
    }

    *mem = x[L - 1];

    return;
}

void Deemph_32_(
     Word16 x_hi[],      /* (i)     : input signal (bit31..16) */
     Word16 x_lo[],      /* (i)     : input signal (bit15..4)  */
     Word16 y[],         /* (o)     : output signal (x16)      */
     Word16 mu,          /* (i) Q15 : deemphasis factor        */
     Word16 L,           /* (i)     : vector size              */
     Word16 * mem        /* (i/o)   : memory (y[-1])           */
)
{
    Word16 i;
    Word32 L_tmp;

    /* L_tmp = hi<<16 + lo<<4 */

    L_tmp = (Word32)x_hi[0] << 16;
    L_tmp = L_tmp + ((Word32)x_lo[0] << 4);
    L_tmp = L_shl(L_tmp, 4);
    L_tmp = L_mac(L_tmp, *mem, mu);          /* saturation can occur here */
    y[0] = round16(L_tmp);

    for (i = 1; i < L; i++)
    {
        L_tmp = (Word32)x_hi[i] << 16;
        L_tmp = L_tmp + ((Word32)x_lo[i] << 4);
        L_tmp = L_shl(L_tmp, 4);
        L_tmp = L_mac(L_tmp, y[i - 1], mu);     /* saturation can occur here */
        y[i] = round16(L_tmp);
    }

    *mem = y[L - 1];

    return;
}
