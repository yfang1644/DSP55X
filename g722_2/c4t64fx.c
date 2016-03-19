/*------------------------------------------------------------------------*
 *                         C4T64FX.C									  *
 *------------------------------------------------------------------------*
 * Performs algebraic codebook search for higher modes	                  *
 *------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*
 * Function  ACELP_4t64_fx()                                             *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~                                             *
 * 20, 36, 44, 52, 64, 72, 88 bits algebraic codebook.                   *
 * 4 tracks x 16 positions per track = 64 samples.                       *
 *                                                                       *
 * 20 bits --> 4 pulses in a frame of 64 samples.                        *
 * 36 bits --> 8 pulses in a frame of 64 samples.                        *
 * 44 bits --> 10 pulses in a frame of 64 samples.                       *
 * 52 bits --> 12 pulses in a frame of 64 samples.                       *
 * 64 bits --> 16 pulses in a frame of 64 samples.                       *
 * 72 bits --> 18 pulses in a frame of 64 samples.                       *
 * 88 bits --> 24 pulses in a frame of 64 samples.                       *
 *                                                                       *
 * All pulses can have two (2) possible amplitudes: +1 or -1.            *
 * Each pulse can have sixteen (16) possible positions.                  *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "acelp.h"
#include "cnst.h"

#include "q_pulse.h"

Word16 tipos[36] = {
    0, 1, 2, 3,              /* starting point &ipos[0], 1st iter */
    1, 2, 3, 0,              /* starting point &ipos[4], 2nd iter */
    2, 3, 0, 1,              /* starting point &ipos[8], 3rd iter */
    3, 0, 1, 2,              /* starting point &ipos[12], 4th iter */
    0, 1, 2, 3,
    1, 2, 3, 0,
    2, 3, 0, 1,
    3, 0, 1, 2,
    0, 1, 2, 3};             /* end point for 24 pulses &ipos[35], 4th iter */

#define NB_PULSE_MAX  24

#define NB_TRACK  4
#define STEP      4
#define NB_POS    16
#define MSIZE     256
#define NB_MAX    8
#define NPMAXPT   ((NB_PULSE_MAX+NB_TRACK-1)/NB_TRACK)


/* locals functions */

/*-------------------------------------------------------------------*
 * Function  cor_h_vec()                                             *
 * ~~~~~~~~~~~~~~~~~~~~~                                             *
 * Compute correlations of h[] with vec[] for the specified track.   *
 *-------------------------------------------------------------------*/
void cor_h_vec(
     Word16 h[],           /* (i) scaled impulse response                 */
     Word16 vec[],         /* (i) scaled vector (/8) to correlate with h[] */
     Word16 track,         /* (i) track to use                            */
     Word16 sign[],        /* (i) sign vector                             */
     Word16 rrixix[][NB_POS],   /* (i) correlation of h[x] with h[x]      */
     Word16 cor[]          /* (o) result of correlation (NB_POS elements) */
)
{
    Word16 i, j, pos, corr;
    Word16 *p0, *p1, *p2;
    Word32 L_sum;

    p0 = rrixix[track];

    pos = track;
    for (i = 0; i < NB_POS; i++, pos += STEP)
    {
        p1 = h;
        p2 = &vec[pos];
        L_sum = 0;
        for (j = pos; j < L_SUBFR; j++)
            L_sum = L_mac(L_sum, *p1++, *p2++);

        L_sum = L_shl(L_sum, 1);

        corr = round16(L_sum);

        cor[i] = add(mult(corr, sign[pos]), *p0++);

    }

    return;
}


/*-------------------------------------------------------------------*
 * Function  search_ixiy()                                           *
 * ~~~~~~~~~~~~~~~~~~~~~~~                                           *
 * Find the best positions of 2 pulses in a subframe.                *
 *-------------------------------------------------------------------*/

void search_ixiy(
     Word16 nb_pos_ix,      /* (i) nb of pos for pulse 1 (1..8)       */
     Word16 track_x,        /* (i) track of pulse 1                   */
     Word16 track_y,        /* (i) track of pulse 2                   */
     Word16 * ps,           /* (i/o) correlation of all fixed pulses  */
     Word16 * alp,          /* (i/o) energy of all fixed pulses       */
     Word16 * ix,           /* (o) position of pulse 1                */
     Word16 * iy,           /* (o) position of pulse 2                */
     Word16 dn[],           /* (i) corr. between target and h[]       */
     Word16 dn2[],          /* (i) vector of selected positions       */
     Word16 cor_x[],        /* (i) corr. of pulse 1 with fixed pulses */
     Word16 cor_y[],        /* (i) corr. of pulse 2 with fixed pulses */
     Word16 rrixiy[][MSIZE] /* (i) corr. of pulse 1 with pulse 2   */
)
{
    Word16 x, y, pos, thres_ix;
    Word16 ps1, ps2, sq, sqk;
    Word16 alp_16, alpk;
    Word16 *p0, *p1, *p2;
    Word32 s, alp0, alp1, alp2;

    p0 = cor_x;
    p1 = cor_y;
    p2 = rrixiy[track_x];

    thres_ix = sub(nb_pos_ix, NB_MAX);

	x = *alp;
    alp0 = (Word32)x << 16;
    alp0 = L_add(alp0, 0x00008000L);           /* for rounding */

    sqk = -1;
    alpk = 1;

    for (x = track_x; x < L_SUBFR; x += STEP)
    {
        ps1 = add(*ps, dn[x]);
        alp1 = L_mac(alp0, *p0++, 4096);

        if (dn2[x] < thres_ix)
        {
            pos = -1;
            for (y = track_y; y < L_SUBFR; y += STEP)
            {
                ps2 = add(ps1, dn[y]);
                alp2 = L_mac(alp1, *p1++, 4096);
                alp2 = L_mac(alp2, *p2++, 8192);
                alp_16 = (Word16)(alp2 >> 16);

                sq = mult(ps2, ps2);

                s = L_msu(L_mult(alpk, sq), sqk, alp_16);

                if (s > 0)
                {
                    sqk = sq;
                    alpk = alp_16;
                    pos = y;
                }
            }
            p1 -= NB_POS;

            if (pos >= 0)
            {
                *ix = x;
                *iy = pos;
            }
        } else
        {
            p2 += NB_POS;
        }
    }

    *ps = add(*ps, add(dn[*ix], dn[*iy]));
    *alp = alpk;

    return;
}


void ACELP_4t64_fx(
     Word16 dn[],   /* (i) <12b : correlation between target x[] and H[]     */
     Word16 cn[],   /* (i) <12b : residual after long term prediction        */
     Word16 H[],    /* (i) Q12: impulse response of weighted synthesis filter*/
     Word16 code[], /* (o) Q9 : algebraic (fixed) codebook excitation        */
     Word16 y[],    /* (o) Q9 : filtered fixed codebook excitation           */
     Word16 nbbits, /* (i) : 20, 36, 44, 52, 64, 72 or 88 bits               */
     Word16 ser_size,   /* (i) : bit rate                                    */
     Word16 _index[]    /* (o) : index (20): 5+5+5+5 = 20 bits.              */
                        /* (o) : index (36): 9+9+9+9 = 36 bits.              */
                        /* (o) : index (44): 13+9+13+9 = 44 bits.            */
                        /* (o) : index (52): 13+13+13+13 = 52 bits.          */
                        /* (o) : index (64): 2+2+2+2+14+14+14+14 = 64 bits.  */
                        /* (o) : index (72): 10+2+10+2+10+14+10+14 = 72 bits.*/
                        /* (o) : index (88): 11+11+11+11+11+11+11+11 = 88 bits.   */
)
{
    Word16 i, j, k, st, ix = 0, iy = 0, pos, index, track, nb_pulse, nbiter;
    Word16 psk, ps, alpk, alp, val, k_cn, k_dn, exp;
    Word16 *p0, *p1, *p2, *p3, *psign;
    Word16 *h, *h_inv, *ptr_h1, *ptr_h2, *ptr_hf, h_shift;
    Word32 s, cor, L_tmp, L_index;

    Word16 dn2[L_SUBFR], sign[L_SUBFR], vec[L_SUBFR];
    Word16 ind[NPMAXPT * NB_TRACK];
    Word16 codvec[NB_PULSE_MAX], nbpos[10];
    Word16 cor_x[NB_POS], cor_y[NB_POS], pos_max[NB_TRACK];
    Word16 h_buf[4 * L_SUBFR];
    Word16 rrixix[NB_TRACK][NB_POS], rrixiy[NB_TRACK][MSIZE];
    Word16 ipos[NB_PULSE_MAX];

    switch (nbbits)
    {
    case 20:                               /* 20 bits, 4 pulses, 4 tracks */
        nbiter = 4;                        /* 4x16x16=1024 loop */
        alp = 8192;                        /* alp = 2.0 (Q12) */
        nb_pulse = 4;
        nbpos[0] = 4;
        nbpos[1] = 8;
        break;
    case 36:                               /* 36 bits, 8 pulses, 4 tracks */
        nbiter = 4;                        /* 4x20x16=1280 loop */
        alp = 4096;                        /* alp = 1.0 (Q12) */
        nb_pulse = 8;
        nbpos[0] = 4;
        nbpos[1] = 8;
        nbpos[2] = 8;
        break;
    case 44:                               /* 44 bits, 10 pulses, 4 tracks */
        nbiter = 4;                        /* 4x26x16=1664 loop */
        alp = 4096;                        /* alp = 1.0 (Q12) */
        nb_pulse = 10;
        nbpos[0] = 4;
        nbpos[1] = 6;
        nbpos[2] = 8;
        nbpos[3] = 8;
        break;
    case 52:                               /* 52 bits, 12 pulses, 4 tracks */
        nbiter = 4;                        /* 4x26x16=1664 loop */
        alp = 4096;                        /* alp = 1.0 (Q12) */
        nb_pulse = 12;
        nbpos[0] = 4;
        nbpos[1] = 6;
        nbpos[2] = 8;
        nbpos[3] = 8;
        break;
    case 64:                               /* 64 bits, 16 pulses, 4 tracks */
        nbiter = 3;                        /* 3x36x16=1728 loop */
        alp = 3277;                        /* alp = 0.8 (Q12) */
        nb_pulse = 16;
        nbpos[0] = 4;
        nbpos[1] = 4;
        nbpos[2] = 6;
        nbpos[3] = 6;
        nbpos[4] = 8;
        nbpos[5] = 8;
        break;
    case 72:                               /* 72 bits, 18 pulses, 4 tracks */
        nbiter = 3;                        /* 3x35x16=1680 loop */
        alp = 3072;                        /* alp = 0.75 (Q12) */
        nb_pulse = 18;
        nbpos[0] = 2;
        nbpos[1] = 3;
        nbpos[2] = 4;
        nbpos[3] = 5;
        nbpos[4] = 6;
        nbpos[5] = 7;
        nbpos[6] = 8;
        break;
    case 88:                               /* 88 bits, 24 pulses, 4 tracks */
        if (sub(ser_size, 462) > 0)
            nbiter = 1;
        else
            nbiter = 2;                    /* 2x53x16=1696 loop */

        alp = 2048;                        /* alp = 0.5 (Q12) */
        nb_pulse = 24;
        nbpos[0] = 2;
        nbpos[1] = 2;
        nbpos[2] = 3;
        nbpos[3] = 4;
        nbpos[4] = 5;
        nbpos[5] = 6;
        nbpos[6] = 7;
        nbpos[7] = 8;
        nbpos[8] = 8;
        nbpos[9] = 8;
        break;
    default:
        nbiter = 0;
        alp = 0;
        nb_pulse = 0;
    }

    for (i = 0; i < nb_pulse; i++)
    {
        codvec[i] = i;
    }

    /*----------------------------------------------------------------*
     * Find sign for each pulse position.                             *
     *----------------------------------------------------------------*/

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
        ps = (Word16)((L_shl(s, 8)) >> 16);

        if (ps >= 0)
        {
            sign[i] = 32767;           /* sign = +1 (Q12) */
            vec[i] = -32768;
        } else
        {
            sign[i] = -32768;          /* sign = -1 (Q12) */
            vec[i] = 32767;
            dn[i] = negate(dn[i]); /* modify dn[] according to the fixed sign */
        }
        dn2[i] = abs_s(ps);          /* dn2[] = mix of dn[] and cn[]            */
    }

    /*----------------------------------------------------------------*
     * Select NB_MAX position per track according to max of dn2[].    *
     *----------------------------------------------------------------*/

    for (i = 0; i < NB_TRACK; i++)
    {
        for (k = 0; k < NB_MAX; k++)
        {
            ps = -1;
            for (j = i; j < L_SUBFR; j += STEP)
            {
                if (dn2[j] > ps)
                {
                    ps = dn2[j];
                    pos = j;
                }
            }

            dn2[pos] = k - NB_MAX;     /* dn2 < 0 when position is selected */
            if (k == 0)
            {
                pos_max[i] = pos;
            }
        }
    }

    /*--------------------------------------------------------------*
     * Scale h[] to avoid overflow and to get maximum of precision  *
     * on correlation.                                              *
     *                                                              *
     * Maximum of h[] (h[0]) is fixed to 2048 (MAX16 / 16).         *
     *  ==> This allow addition of 16 pulses without saturation.    *
     *                                                              *
     * Energy worst case (on resonant impulse response),            *
     * - energy of h[] is approximately MAX/16.                     *
     * - During search, the energy is divided by 8 to avoid         *
     *   overflow on "alp". (energy of h[] = MAX/128).              *
     *  ==> "alp" worst case detected is 22854 on sinusoidal wave.  *
     *--------------------------------------------------------------*/

    /* impulse response buffer for fast computation */

    h = h_buf;
    h_inv = h_buf + (2 * L_SUBFR);
    for (i = 0; i < L_SUBFR; i++)
    {
        *h++ = 0;
        *h_inv++ = 0;
    }

    /* scale h[] down (/2) when energy of h[] is high with many pulses used */
    L_tmp = Dot_product(H, H, L_SUBFR);
    val = (Word16)(L_tmp >> 16);

    h_shift = 0;

    if ((nb_pulse >= 12) && (val > 1024))
    {
        h_shift = 1;
    }
    for (i = 0; i < L_SUBFR; i++)
    {
        h[i] = H[i] >> h_shift;
        h_inv[i] = negate(h[i]);
    }

    /*------------------------------------------------------------*
     * Compute rrixix[][] needed for the codebook search.         *
     * This algorithm compute impulse response energy of all      *
     * positions (16) in each track (4).       Total = 4x16 = 64. *
     *------------------------------------------------------------*/

    /* storage order --> i3i3, i2i2, i1i1, i0i0 */

    /* Init pointers to last position of rrixix[] */
    p0 = &rrixix[0][NB_POS - 1];
    p1 = &rrixix[1][NB_POS - 1];
    p2 = &rrixix[2][NB_POS - 1];
    p3 = &rrixix[3][NB_POS - 1];

    ptr_h1 = h;
    cor = 0x00008000L;                      /* for rounding */
    for (i = 0; i < NB_POS; i++)
    {
        cor = L_mac(cor, *ptr_h1, *ptr_h1);
        ptr_h1++;
        *p3-- = (Word16)(cor >> 16);
        cor = L_mac(cor, *ptr_h1, *ptr_h1);
        ptr_h1++;
        *p2-- = (Word16)(cor >> 16);
        cor = L_mac(cor, *ptr_h1, *ptr_h1);
        ptr_h1++;
        *p1-- = (Word16)(cor >> 16);
        cor = L_mac(cor, *ptr_h1, *ptr_h1);
        ptr_h1++;
        *p0-- = (Word16)(cor >> 16);
    }

    /*------------------------------------------------------------*
     * Compute rrixiy[][] needed for the codebook search.         *
     * This algorithm compute correlation between 2 pulses        *
     * (2 impulses responses) in 4 possible adjacents tracks.     *
     * (track 0-1, 1-2, 2-3 and 3-0).     Total = 4x16x16 = 1024. *
     *------------------------------------------------------------*/

    /* storage order --> i2i3, i1i2, i0i1, i3i0 */

    pos = MSIZE - 1;
    ptr_hf = h + 1;

    for (k = 0; k < NB_POS; k++)
    {
        p3 = &rrixiy[2][pos];
        p2 = &rrixiy[1][pos];
        p1 = &rrixiy[0][pos];
        p0 = &rrixiy[3][pos - NB_POS];

        ptr_h1 = h;
        ptr_h2 = ptr_hf;

        cor = 0x00008000L;                   /* for rounding */
        for (i = k + 1; i < NB_POS; i++)
        {
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p3 = (Word16)(cor >> 16);
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p2 = (Word16)(cor >> 16);
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p1 = (Word16)(cor >> 16);
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p0 = (Word16)(cor >> 16);

            p3 -= (NB_POS + 1);
            p2 -= (NB_POS + 1);
            p1 -= (NB_POS + 1);
            p0 -= (NB_POS + 1);
        }
        cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
        *p3 = (Word16)(cor >> 16);
        cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
        *p2 = (Word16)(cor >> 16);
        cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
        *p1 = (Word16)(cor >> 16);

        pos -= NB_POS;
        ptr_hf += STEP;
    }

    /* storage order --> i3i0, i2i3, i1i2, i0i1 */

    pos = MSIZE - 1;
    ptr_hf = h + 3;

    for (k = 0; k < NB_POS; k++)
    {
        p3 = &rrixiy[3][pos];
        p2 = &rrixiy[2][pos - 1];
        p1 = &rrixiy[1][pos - 1];
        p0 = &rrixiy[0][pos - 1];

        ptr_h1 = h;
        ptr_h2 = ptr_hf;

        cor = 0x00008000L;                  /* for rounding */
        for (i = k + 1; i < NB_POS; i++)
        {
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p3 = (Word16)(cor >> 16);
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p2 = (Word16)(cor >> 16);
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p1 = (Word16)(cor >> 16);
            cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
            *p0 = (Word16)(cor >> 16);

            p3 -= (NB_POS + 1);
            p2 -= (NB_POS + 1);
            p1 -= (NB_POS + 1);
            p0 -= (NB_POS + 1);
        }
        cor = L_mac(cor, *ptr_h1++, *ptr_h2++);
        *p3 = (Word16)(cor >> 16);

        pos--;
        ptr_hf += STEP;
    }

    /*------------------------------------------------------------*
     * Modification of rrixiy[][] to take signs into account.     *
     *------------------------------------------------------------*/

    p0 = &rrixiy[0][0];

    for (k = 0; k < NB_TRACK; k++)
    {
        for (i = k; i < L_SUBFR; i += STEP)
        {
            psign = sign;
            if (psign[i] < 0)
            {
                psign = vec;
            }
            for (j = (Word16) ((k + 1) % NB_TRACK); j < L_SUBFR; j += STEP)
            {
                *p0 = mult(*p0, psign[j]);
				p0++;
            }
        }
    }

    /*-------------------------------------------------------------------*
     *                       Deep first search                           *
     *-------------------------------------------------------------------*/

    psk = -1;
    alpk = 1;

    for (k = 0; k < nbiter; k++)
    {
        for (i = 0; i < nb_pulse; i++)
            ipos[i] = tipos[(k * 4) + i];

        if (nbbits == 20)
        {
            pos = 0;
            ps = 0;
            alp = 0;
            for (i = 0; i < L_SUBFR; i++)
            {
                vec[i] = 0;
            }
        } else if ((nbbits == 36) || (nbbits == 44))
        {
            /* first stage: fix 2 pulses */
            pos = 2;

            ix = ind[0] = pos_max[ipos[0]];
            iy = ind[1] = pos_max[ipos[1]];
            ps = add(dn[ix], dn[iy]);
            i = shr(ix, 2);                /* ix / STEP */
            j = shr(iy, 2);                /* iy / STEP */
            s = L_mult(rrixix[ipos[0]][i], 4096);
            s = L_mac(s, rrixix[ipos[1]][j], 4096);
            i = add(shl(i, 4), j);         /* (ix/STEP)*NB_POS + (iy/STEP) */
            s = L_mac(s, rrixiy[ipos[0]][i], 8192);
            alp = round16(s);
            if (sign[ix] < 0)
                p0 = h_inv - ix;
            else
                p0 = h - ix;

            if (sign[iy] < 0)
                p1 = h_inv - iy;
            else
                p1 = h - iy;

            for (i = 0; i < L_SUBFR; i++)
            {
                vec[i] = add(*p0++, *p1++);
            }

            if (nbbits == 44)
            {
                ipos[8] = 0;
                ipos[9] = 1;
            }
        } else
        {
            /* first stage: fix 4 pulses */
            pos = 4;

            ix = ind[0] = pos_max[ipos[0]];
            iy = ind[1] = pos_max[ipos[1]];
            i = ind[2] = pos_max[ipos[2]];
            j = ind[3] = pos_max[ipos[3]];
            ps = add(add(add(dn[ix], dn[iy]), dn[i]), dn[j]);

            if (sign[ix] < 0)
                p0 = h_inv - ix;
            else
                p0 = h - ix;

            if (sign[iy] < 0)
                p1 = h_inv - iy;
            else
                p1 = h - iy;

            if (sign[i] < 0)
                p2 = h_inv - i;
            else
                p2 = h - i;

            if (sign[j] < 0)
                p3 = h_inv - j;
            else
                p3 = h - j;

            for (i = 0; i < L_SUBFR; i++)
            {
                vec[i] = add(add(add(*p0++, *p1++), *p2++), *p3++);
            }

            L_tmp = Dot_product(vec, vec, L_SUBFR);
            alp = round16(L_tmp >> 3);

            if (nbbits == 72)
            {
                ipos[16] = 0;
                ipos[17] = 1;
            }
        }

        /* other stages of 2 pulses */

        for (j = pos, st = 0; j < nb_pulse; j += 2, st++)
        {
            /*--------------------------------------------------*
            * Calculate correlation of all possible positions  *
            * of the next 2 pulses with previous fixed pulses. *
            * Each pulse can have 16 possible positions.       *
            *--------------------------------------------------*/

            cor_h_vec(h, vec, ipos[j], sign, rrixix, cor_x);
            cor_h_vec(h, vec, ipos[j + 1], sign, rrixix, cor_y);

            /*--------------------------------------------------*
            * Find best positions of 2 pulses.                 *
            *--------------------------------------------------*/

            search_ixiy(nbpos[st], ipos[j], ipos[j + 1], &ps, &alp,
                &ix, &iy, dn, dn2, cor_x, cor_y, rrixiy);

            ind[j] = ix;
            ind[j + 1] = iy;

            if (sign[ix] < 0)
                p0 = h_inv - ix;
            else
                p0 = h - ix;

            if (sign[iy] < 0)
                p1 = h_inv - iy;
            else
                p1 = h - iy;

            for (i = 0; i < L_SUBFR; i++)
            {
                vec[i] = add(vec[i], add(*p0++, *p1++));  /* can saturate here. */
            }
        }

        /* memorise the best codevector */

        ps = mult(ps, ps);
        s = L_msu(L_mult(alpk, ps), psk, alp);
        if (s > 0)
        {
            psk = ps;
            alpk = alp;
            for (i = 0; i < nb_pulse; i++)
            {
                codvec[i] = ind[i];
            }
            for (i = 0; i < L_SUBFR; i++)
            {
                y[i] = vec[i];
            }
        }
    }

    /*-------------------------------------------------------------------*
     * Build the codeword, the filtered codeword and index of codevector.*
     *-------------------------------------------------------------------*/

    for (i = 0; i < NPMAXPT * NB_TRACK; i++)
    {
        ind[i] = -1;
    }
    for (i = 0; i < L_SUBFR; i++)
    {
        code[i] = 0;
        y[i] = (y[i] + 4) >> 3;               /* Q12 to Q9, with rounding */
    }

    val = shr(512, h_shift);               /* codeword in Q9 format */

    for (k = 0; k < nb_pulse; k++)
    {
        i = codvec[k];                       /* read pulse position */
        j = sign[i];                         /* read sign           */

        index = i >> 2;                 /* index = pos of pulse (0..15) */
        track = i & 0x03;              /* track = i % NB_TRACK (0..3)  */

        if (j > 0)
        {
            code[i] = add(code[i], val);
            codvec[k] = add(codvec[k], (2 * L_SUBFR));
        } else
        {
            code[i] = sub(code[i], val);
            index = add(index, NB_POS);
        }

        L_tmp = (Word32)track * (Word32)NPMAXPT;
        i = (Word16) L_tmp;

        while (ind[i] >= 0)
        {
            i++;
        }
        ind[i] = index;
    }

    k = 0;
    /* Build index of codevector */
    switch(nbbits)
    {
    case 20:
        for (track = 0; track < NB_TRACK; track++)
        {
            _index[track] = (Word16) (quant_1p_N1(ind[k], 4));
            k += NPMAXPT;
        }
        break;
    case 36:
        for (track = 0; track < NB_TRACK; track++)
        {
            _index[track] = (Word16) (quant_2p_2N1(ind[k], ind[k + 1], 4));
            k += NPMAXPT;
        }
        break;
    case 44:
        for (track = 0; track < NB_TRACK - 2; track++)
        {
            _index[track] = (Word16) (quant_3p_3N1(ind[k], ind[k + 1], ind[k + 2], 4));
            k += NPMAXPT;
        }
        for (track = 2; track < NB_TRACK; track++)
        {
            _index[track] = (Word16) (quant_2p_2N1(ind[k], ind[k + 1], 4));
            k += NPMAXPT;
        }
        break;
    case 52:
        for (track = 0; track < NB_TRACK; track++)
        {
            _index[track] = (Word16) (quant_3p_3N1(ind[k], ind[k + 1], ind[k + 2], 4));
            k += NPMAXPT;
        }
        break;
    case 64:
        for (track = 0; track < NB_TRACK; track++)
        {
            L_index = quant_4p_4N(&ind[k], 4);
            _index[track] = (Word16) ((L_index >> 14) & 3);
            _index[track + NB_TRACK] = (Word16) (L_index & 0x3fff);
            k += NPMAXPT;
        }
        break;
    case 72:
        for (track = 0; track < NB_TRACK - 2; track++)
        {
            L_index = quant_5p_5N(&ind[k], 4);
            _index[track] = (Word16) ((L_index >> 10) & 0x03ff);
            _index[track + NB_TRACK] = (Word16) (L_index & 0x03ff);
            k += NPMAXPT;
        }
        for (track = 2; track < NB_TRACK; track++)
        {
            L_index = quant_4p_4N(&ind[k], 4);
            _index[track] = (Word16) ((L_index >> 14) & 3);
            _index[track + NB_TRACK] = (Word16) (L_index & 0x3fff);
            k += NPMAXPT;
        }
        break;
    case 88:
        for (track = 0; track < NB_TRACK; track++)
        {
            L_index = quant_6p_6N_2(&ind[k], 4);
            _index[track] = (Word16) ((L_index >> 11) & 0x07ff);
            _index[track + NB_TRACK] = (Word16) (L_index & 0x07ff);
            k += NPMAXPT;
        }
        break;
    default:
        break;
    }
    return;
}

