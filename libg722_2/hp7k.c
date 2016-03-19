/*-------------------------------------------------------------------*
 *                         HP6K.C									 *
 *-------------------------------------------------------------------*
 * 15th order high pass 7kHz FIR filter.                             *
 *                                                                   *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "cnst.h"
#include "basic_op.h"
#include "math_op.h"

#define L_FIR 31

Word16 fir_7k[L_FIR] =
{
    -21, 47, -89, 146, -203,
    229, -177, 0, 335, -839,
    1485, -2211, 2931, -3542, 3953,
    28682, 3953, -3542, 2931, -2211,
    1485, -839, 335, 0, -177,
    229, -203, 146, -89, 47,
    -21
};


void Filt_7k(
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
        x[i + L_FIR - 1] = signal[i];
        L_tmp = Dot_product(&x[i], fir_7k, L_FIR); 

        signal[i] = round16(L_tmp);
    }

    for(i = 0; i < L_FIR - 1; i++)
    {
        mem[i] = x[lg + i];
    }

    return;
}
