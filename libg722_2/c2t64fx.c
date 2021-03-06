/*------------------------------------------------------------------------*
 *                         C2T64FX.C                                      *
 *------------------------------------------------------------------------*
 * Performs algebraic codebook search for 6.60 kbit/s mode                *
 *------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*
 * Function  ACELP_2t64_fx()                                             *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~                                             *
 * 12 bits algebraic codebook.                                           *
 * 2 tracks x 32 positions per track = 64 samples.                       *
 *                                                                       *
 * 12 bits --> 2 pulses in a frame of 64 samples.                        *
 *                                                                       *
 * All pulses can have two (2) possible amplitudes: +1 or -1.            *
 * Each pulse can have 32 possible positions.                            *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "acelp.h"
#include "cnst.h"

#define NB_TRACK  2
#define STEP      2
#define NB_POS    32
#define MSIZE     1024


void ACELP_2t64_fx(
     Word16 dn[],      /* (i) <12b : correlation between target x[] and H[]  */
     Word16 cn[],      /* (i) <12b : residual after long term prediction     */
     Word16 H[],       /* (i) Q12: impulse response of weighted synthesis filter */
     Word16 code[],    /* (o) Q9 : algebraic (fixed) codebook excitation     */
     Word16 y[],       /* (o) Q9 : filtered fixed codebook excitation        */
     Word16 * index    /* (o) : index (12): 5+1+5+1 = 11 bits.               */
)
{
    Word16 i, j, k, i0, i1, ix, iy, pos, pos2;
    Word16 psk, ps1, ps2, alpk, alp1, alp2, sq;
    Word16 alp, exp, k_cn, k_dn;
    Word16 *p0, *p1, *p2, *psign;
    Word16 *h, *h_inv, *ptr_h1, *ptr_h2, *ptr_hf;

    Word16 sign[L_SUBFR], vec[L_SUBFR], dn2[L_SUBFR];
    Word16 h_buf[4 * L_SUBFR];
    Word16 rrixix[NB_TRACK][NB_POS];
    Word16 rrixiy[MSIZE];
    Word32 s, cor;

    /*----------------------------------------------------------------*
     * Find sign for each pulse position.                             *
     *----------------------------------------------------------------*/

    alp = 8192;                             /* alp = 2.0 (Q12) */

    /* calculate energy for normalization of cn[] and dn[] */

    /* set k_cn = 32..32767 (ener_cn = 2^30..256-0) */
    s = Dot_product12(cn, cn, L_SUBFR, &exp);
    s = Isqrt_n(s, &exp);
    exp = -exp - 15 + 5;
    s = L_shl(s, exp);                     /* saturation can occur here */
    k_cn = round16(s);

    /* set k_dn = 32..512 (ener_dn = 2^30..2^22) */
    s = Dot_product12(dn, dn, L_SUBFR, &exp);
    s = Isqrt_n(s, &exp);
    exp = -exp - 15 + 5 + 3;
    k_dn = round16(L_shl(s, exp));         /* k_dn = 256..4096 */
    k_dn = mult_r(alp, k_dn);              /* alp in Q12 */

    /* set sign according to dn2[] = k_cn*cn[] + k_dn*dn[]    */

    for (i = 0; i < L_SUBFR; i++)
    {
        /* mix normalized cn[] and dn[] */
        s = L_mult(k_cn, cn[i]);
        s = L_mac(s, k_dn, dn[i]);
        dn2[i] = (Word16)(s >> 8);

        if (dn2[i] >= 0)
        {
            sign[i] = 32767;             /* sign = +1 (Q12) */
            vec[i] = -32768;
        } else
        {
            sign[i] = -32768;            /* sign = -1 (Q12) */
            vec[i] = 32767;
            dn[i] = negate(dn[i]); /* modify dn[] according to the fixed sign */
        }
    }

    /*------------------------------------------------------------*
     * Compute h_inv[i].                                          *
     *------------------------------------------------------------*/

    /* impulse response buffer for fast computation */

    h = h_buf;
    h_inv = h_buf + (2 * L_SUBFR);
    for (i = 0; i < L_SUBFR; i++)
    {
        *h++ = 0;
        *h_inv++ = 0;
    }

    for (i = 0; i < L_SUBFR; i++)
    {
        h[i] = H[i];
        h_inv[i] = negate(h[i]);
    }

    /*------------------------------------------------------------*
     * Compute rrixix[][] needed for the codebook search.         *
     * Result is multiplied by 0.5                                *
     *------------------------------------------------------------*/

    /* Init pointers to last position of rrixix[] */
    p0 = &rrixix[0][NB_POS - 1];
    p1 = &rrixix[1][NB_POS - 1];

    ptr_h1 = h;
    cor = 0x00010000L;             /* for rounding */
    for (i = 0; i < NB_POS; i++)
    {
        cor = L_mac(cor, *ptr_h1, *ptr_h1);
        ptr_h1++;
        *p1-- = (Word16)(cor >> 16);
        cor = L_mac(cor, *ptr_h1, *ptr_h1);
        ptr_h1++;
        *p0-- = (Word16)(cor >> 16);
    }

    p0 = rrixix[0];
    p1 = rrixix[1];
    for (i = 0; i < NB_POS; i++)
    {
        *p0++ >>= 1;	
        *p1++ >>= 1;
    }

    /*------------------------------------------------------------*
     * Compute rrixiy[][] needed for the codebook search.         *
     *------------------------------------------------------------*/

    pos = MSIZE - 1;
    pos2 = MSIZE - 2;
    ptr_hf = h + 1;

    for (k = 0; k < NB_POS; k++)
    {
        p1 = &rrixiy[pos];
        p0 = &rrixiy[pos2];

        cor = 0x00008000L;               /* for rounding */
        ptr_h1 = h;
        ptr_h2 = ptr_hf;

        for (i = (k + 1); i < NB_POS; i++)
        {
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p1 = (Word16)(cor >> 16);
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p0 = (Word16)(cor >> 16);

            p1 -= (NB_POS + 1);
            p0 -= (NB_POS + 1);
        }
        cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
        *p1 = (Word16)(cor >> 16);

        pos -= NB_POS;
        pos2--;
        ptr_hf += STEP;
    }

    /*------------------------------------------------------------*
     * Modification of rrixiy[][] to take signs into account.     *
     *------------------------------------------------------------*/

    p0 = rrixiy;

    for (i = 0; i < L_SUBFR; i += STEP)
    {
        psign = sign;
        if (psign[i] < 0)
        {
            psign = vec;
        }
        for (j = 1; j < L_SUBFR; j += STEP)
        {
            *p0 = mult(*p0, psign[j]);
			p0++;
        }
    }

    /*-------------------------------------------------------------------*
     * search 2 pulses:                                                  *
     * ~@~~~~~~~~~~~~~~                                                  *
     * 32 pos x 32 pos = 1024 tests (all combinaisons is tested)         *
     *-------------------------------------------------------------------*/

    p0 = rrixix[0];
    p1 = rrixix[1];
    p2 = rrixiy;

    psk = -1;
    alpk = 1;
    ix = 0;
    iy = 1;

    for (i0 = 0; i0 < L_SUBFR; i0 += STEP)
    {
        ps1 = dn[i0];
        alp1 = (*p0++);

        pos = -1;
        for (i1 = 1; i1 < L_SUBFR; i1 += STEP)
        {
            ps2 = add(ps1, dn[i1]);
            alp2 = add(alp1, add(*p1++, *p2++));

            sq = mult(ps2, ps2);

            s = L_msu(L_mult(alpk, sq), psk, alp2);

            if (s > 0)
            {
                psk = sq;
                alpk = alp2;
                pos = i1;
            }
        }
        p1 -= NB_POS;

        if (pos >= 0)
        {
            ix = i0;
            iy = pos;
        }
    }

    /*-------------------------------------------------------------------*
     * Build the codeword, the filtered codeword and index of codevector.*
     *-------------------------------------------------------------------*/

    for (i = 0; i < L_SUBFR; i++)
    {
        code[i] = 0;
    }

    i0 = ix >> 1;                       /* pos of pulse 1 (0..31) */
    i1 = iy >> 1;                       /* pos of pulse 2 (0..31) */

    if (sign[ix] > 0)
    {
        code[ix] = 512;                     /* codeword in Q9 format */
        p0 = h - ix;
    } else
    {
        code[ix] = -512;
        i0 += NB_POS;
        p0 = h_inv - ix;
    }

    if (sign[iy] > 0)
    {
        code[iy] = 512;
        p1 = h - iy;
    } else
    {
        code[iy] = -512;
        i1 += NB_POS;
        p1 = h_inv - iy;
    }

    *index = add(shl(i0, 6), i1);

    for (i = 0; i < L_SUBFR; i++)
    {
        s = ((Word32)*p0++) + ((Word32)*p1++);
        s = (s + 4) >> 3;  
        y[i] = (Word16)s;
//        y[i] = shr_r(add(*p0++, *p1++), 3);
    }

    return;
}
