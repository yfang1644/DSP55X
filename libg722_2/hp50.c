/*-----------------------------------------------------------------------*
 *                         HP50.C										 *
 *-----------------------------------------------------------------------*
 * 2nd order high pass filter with cut off frequency at 31 Hz.           *
 * Designed with cheby2 function in MATLAB.                              *
 * Optimized for fixed-point to get the following frequency response:    *
 *                                                                       *
 *  frequency:     0Hz    14Hz  24Hz   31Hz   37Hz   41Hz   47Hz         *
 *  dB loss:     -infdB  -15dB  -6dB   -3dB  -1.5dB  -1dB  -0.5dB        *
 *                                                                       *
 * Algorithm:                                                            *
 *                                                                       *
 *  y[i] = b[0]*x[i] + b[1]*x[i-1] + b[2]*x[i-2]                         *
 *                   + a[1]*y[i-1] + a[2]*y[i-2];                        *
 *                                                                       *
 *  Word16 b[3] = {4053, -8106, 4053};       in Q12                     *
 *  Word16 a[3] = {8192, 16211, -8021};       in Q12                     *
 *                                                                       *
 *  float -->   b[3] = {0.989501953, -1.979003906,  0.989501953};        *
 *              a[3] = {1.000000000,  1.978881836, -0.979125977};        *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"

/* filter coefficients  */
Word16 b50[3] = {4053, -8106, 4053};  /* Q12 */
Word16 a50[3] = {8192, 16211, -8021}; /* Q12 (x2) */

void HP50_12k8(
     Word16 signal[],                      /* input/output signal */
     Word16 lg,                            /* lenght of signal    */
     Word16 mem[]                          /* filter memory [6]   */
)
{
    Word16 i, x2;
    Word16 x0, x1;
    Word32 L_tmp;
	DPF y1, y2;

    y2.hi = mem[0];
    y2.lo = mem[1];
    y1.hi = mem[2];
    y1.lo = mem[3];
    x0 = mem[4];
    x1 = mem[5];

    for (i = 0; i < lg; i++)
    {
        x2 = x1;
        x1 = x0;
        x0 = signal[i];

        /* y[i] = b[0]*x[i] + b[1]*x[i-1] + b[2]*x[i-2]  */
        /*                  + a[1]*y[i-1] + a[2]*y[i-2]; */

        L_tmp = 0x4000L;                 /* rounding to maximise precision */
        L_tmp = L_mac(L_tmp, y1.lo, a50[1]);
        L_tmp = L_mac(L_tmp, y2.lo, a50[2]);
        L_tmp = L_tmp >> 15;
        L_tmp = L_mac(L_tmp, y1.hi, a50[1]);
        L_tmp = L_mac(L_tmp, y2.hi, a50[2]);

        L_tmp = L_mac(L_tmp, x0, b50[0]);
        L_tmp = L_mac(L_tmp, x1, b50[1]);
        L_tmp = L_mac(L_tmp, x2, b50[2]);

        L_tmp = L_shl(L_tmp, 2);           /* coeff Q12 --> Q14 */

        y2 = y1;

		y1.hi = (Word16)(L_tmp >> 16);
		y1.lo = (Word16)(L_tmp >> 1) & 0x7fff;
//		L_Extract(L_tmp, &y1_hi, &y1_lo);

        L_tmp = L_shl(L_tmp, 1);       /* coeff Q14 --> Q15 with saturation */
        signal[i] = round16(L_tmp);
    }

    mem[0] = y2.hi;
    mem[1] = y2.lo;
    mem[2] = y1.hi;
    mem[3] = y1.lo;
    mem[4] = x0;
    mem[5] = x1;

    return;
}
