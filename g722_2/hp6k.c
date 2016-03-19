/*-------------------------------------------------------------------*
 *                         HP6K.C									 *
 *-------------------------------------------------------------------*
 * 15th order band pass 6kHz to 7kHz FIR filter.                     *
 *                                                                   *
 * frequency:  4kHz   5kHz  5.5kHz  6kHz  6.5kHz 7kHz  7.5kHz  8kHz  *
 * dB loss:   -60dB  -45dB  -13dB   -3dB   0dB   -3dB  -13dB  -45dB  *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "cnst.h"
#include "basic_op.h"
#include "math_op.h"

#define L_FIR 31

/* filter coefficients (gain=4.0) */

Word16 fir_6k_7k[L_FIR] =
{
    -32, 47, 32, -27, -369,
    1122, -1421, 0, 3798, -8880,
    12349, -10984, 3548, 7766, -18001,
    22118, -18001, 7766, 3548, -10984,
    12349, -8880, 3798, 0, -1421,
    1122, -369, -27, 32, 47,
    -32
};

void Filt_6k_7k(
     Word16 signal[],                      /* input:  signal                  */
     Word16 lg,                            /* input:  length of input         */
     Word16 mem[]                          /* in/out: memory (size=30)        */
)
{
    Word16 i, x[L_SUBFR16k + (L_FIR - 1)];
    Word32 L_tmp;

    for(i = 0; i < L_FIR - 1; i++)
    {
        x[i] = mem[i];
    }

    for (i = 0; i < lg; i++)
    {
        x[i + L_FIR - 1] = signal[i] >> 2;     /* gain of filter = 4 */
        L_tmp = Dot_product(&x[i], fir_6k_7k, L_FIR);

        signal[i] = round16(L_tmp);
    }

    for(i = 0; i < L_FIR - 1; i++)
    {
        mem[i] = x[lg + i];
    }

    return;
}
