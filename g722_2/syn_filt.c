/*-------------------------------------------------------------------*
 *                         SYN_FILT.C								 *
 *-------------------------------------------------------------------*
 * Do the synthesis filtering 1/A(z).							     *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"

void Syn_filt(
     Word16 a[],       /* (i) Q12 : a[m+1] prediction coefficients           */
     Word16 m,         /* (i)     : order of LP filter                       */
     Word16 x[],       /* (i)     : input signal                             */
     Word16 y[],       /* (o)     : output signal                            */
     Word16 lg,        /* (i)     : size of filtering                        */
     Word16 mem[],     /* (i/o)   : memory associated with this filtering.   */
     Word16 update     /* (i)     : 0=no update, 1=update of memory.         */
)
{
    Word16 i, j, y_buf[L_SUBFR16k + M16k], a0, s;
    Word32 L_tmp;
    Word16 *yy;

    yy = &y_buf[0];

    /* copy initial filter states into synthesis buffer */
    for (i = 0; i < m; i++)
    {
        *yy++ = mem[i];
    }

    s = norm_s0(a[0]);
    s = s + 1;
    a0 = a[0] >> 1;                     /* input / 2 */

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        L_tmp = L_mult(x[i], a0);
        for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, a[j], yy[i - j]);

        L_tmp = L_shl(L_tmp, s);

        y[i] = yy[i] = round16(L_tmp);
    }

    /* Update memory if required */
    if (update)
        for (i = 0; i < m; i++)
        {
            mem[i] = yy[lg - m + i];
        }

    return;
}


void Syn_filt_32(
     Word16 a[],          /* (i) Q12 : a[m+1] prediction coefficients */
     Word16 m,            /* (i)     : order of LP filter             */
     Word16 exc[],        /* (i) Qnew: excitation (exc[i] >> Qnew)    */
     Word16 Qnew,         /* (i)     : exc scaling = 0(min) to 8(max) */
     Word16 sig_hi[],     /* (o) /16 : synthesis high                 */
     Word16 sig_lo[],     /* (o) /16 : synthesis low                  */
     Word16 lg            /* (i)     : size of filtering              */
)
{
    Word16 i, j, a0, s;
    Word32 L_tmp;

    s = norm_s0(a[0]);
    s = s + 1;

    a0 = shr(a[0], add(4, Qnew));          /* input / 16 and >>Qnew */

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        L_tmp = 0;
        for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, sig_lo[i - j], a[j]);

        L_tmp = L_tmp >> (16 - 4);      /* -4 : sig_lo[i] << 4 */

        for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, sig_hi[i - j], a[j]);

        L_tmp = L_mac(L_tmp, exc[i], a0);

        /* sig_hi = bit16 to bit31 of synthesis */
        L_tmp = L_shl(L_tmp, s);           /* ai in Q12 */
        sig_hi[i] = (Word16)(L_tmp >> 16);

        /* sig_lo = bit4 to bit15 of synthesis */
        /* 4 : sig_lo[i] >> 4 */
        sig_lo[i] = (Word16)(L_tmp >> 4) & 0x0fff;
    }

    return;
}
