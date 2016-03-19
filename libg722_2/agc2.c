/*------------------------------------------------------------------------*
 *                         AGC2.C                                         *
 *------------------------------------------------------------------------*
 * Performs adaptive gain control                                         *
 *------------------------------------------------------------------------*/

#include "typedef.h"
#include "cnst.h"
#include "basic_op.h"
#include "math_op.h"

void agc2(
     Word16 * sig_in,             /* (i)     : postfilter input signal  */
     Word16 * sig_out,            /* (i/o)   : postfilter output signal */
     Word16 l_trm                 /* (i)     : subframe size            */
)
{

    Word16 i, exp;
    Word16 gain_in, gain_out, g0;
    Word32 s;

    Word16 temp;

    /* calculate gain_out with exponent */

    s = 0;
    for (i = 0; i < l_trm; i++)
    {
        temp = sig_out[i] >> 2;
        s = L_mac(s, temp, temp);
    }
    if (s == 0)
    {
        return;
    }

    exp = exp_mant0(&s);
    exp = exp + 1;
    s = s >> 1;
    gain_out = round16(s);

    /* calculate gain_in with exponent */

    s = 0;
    for (i = 0; i < l_trm; i++)
    {
        temp = sig_in[i] >> 2;
        s = L_mac(s, temp, temp);
    }
    if (s == 0)
    {
        g0 = 0;
    } else
    {
        i = exp_mant0(&s);
        gain_in = round16(s);
        exp = i - exp;

        /*---------------------------------------------------*
         *  g0 = sqrt(gain_in/gain_out);                     *
         *---------------------------------------------------*/

        s = (Word32)(div_s(gain_out, gain_in));
        s = L_shl(s, 7);                   /* s = gain_out / gain_in */
        s = L_shr(s, exp);                 /* add exponent */

//        s = Isqrt(s);
        exp = exp_mant0(&s);               /*******************/  
        exp = 13 + exp;                    /*  SQRT(s)        */
        s = Isqrt_n(s, &exp);              /*                 */
        s = L_shr(s, exp);                 /*******************/
        g0 = round16(s);
    }
    /* sig_out(n) = gain(n) sig_out(n) */

    for (i = 0; i < l_trm; i++)
    {
        s = L_mult(sig_out[i], g0);
        s = L_shl(s, 2);
        sig_out[i] = (Word16)(s >> 16);
    }

    return;
}

