/*****************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   2004 Polycom, Inc.
**
**   All rights reserved.
**
*****************************************************************************/

/*****************************************************************************
* Filename: rmlt_coefs_to_samples.c
*
* Purpose:  Convert Reversed MLT (Modulated Lapped Transform)
*           Coefficients to Samples
*
*     The "Reversed MLT" is an overlapped block transform which uses
*     even symmetry * on the left, odd symmetry on the right and a
*     Type IV DCT as the block transform.  * It is thus similar to a
*     MLT which uses odd symmetry on the left, even symmetry * on the
*     right and a Type IV DST as the block transform.  In fact, it is
*     equivalent * to reversing the order of the samples, performing
*     an MLT and then negating all * the even-numbered coefficients.
*
*****************************************************************************/

/***************************************************************************
 Include files
***************************************************************************/
#include "codec.h"
#include "defs.h"
#include "tables.h"

#include "basop32.h"

void dct_type_iv_s (Int16 *input,Int16 *output, Int16 dct_length);

/***************************************************************************
 Function:     rmlt_coefs_to_samples 

 Syntax:       void rmlt_coefs_to_samples(Int16 *coefs,       
                                          Int16 *old_samples, 
                                          Int16 *out_samples, 
                                          Int16 mag_shift)    
            
               inputs:    Int16 *coefs
                          Int16 *old_samples
                          Int16 mag_shift


               outputs:   Int16 *out_samples

 Description:  Converts the mlt_coefs to samples

 Design Notes:

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |     1.91     |    1.91
          -------|--------------|----------------
            MAX  |     1.91     |    1.91
          -------|--------------|----------------
				
           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |     3.97     |    3.97        |     3.97
          -------|--------------|----------------|----------------
            MAX  |     3.97     |    3.97        |     3.97
          -------|--------------|----------------|----------------

***************************************************************************/
extern Int16 windowed_data[];
#pragma DATA_ALIGN (old_samples, 2)
static Int16 old_samples[DCT_LENGTH];

void rmlt_coefs_to_samples(Int16 *coefs,     
                           Int16 *out_samples,           
                           Int16 mag_shift)             
{
    Int16	i;
    Int16	*new_ptr, *old_ptr;
    Int16	*win_new, *win_old;
    Int16	*out_ptr;
    Int32  sum;

//	static Int16 old_samples[DCT_LENGTH];

    /* Perform a Type IV (inverse) DCT on the coefficients */
    dct_type_iv_s(coefs, windowed_data, DCT_LENGTH);

    for(i = 0; i < DCT_LENGTH; i++)
        windowed_data[i] = shr(windowed_data[i], mag_shift);

    /* Get the first half of the windowed samples */

    out_ptr = out_samples;
	win_new = rmlt_to_samples_window;
	win_old = rmlt_to_samples_window + DCT_LENGTH;

    old_ptr = old_samples;
    new_ptr = windowed_data + (DCT_LENGTH>>1);

    for (i = 0; i < (DCT_LENGTH>>1); i++)
    {
        sum = L_mult(*win_new++, *--new_ptr);
        sum = L_mac(sum, *--win_old, *old_ptr++);
        *out_ptr++ = round16(L_shl(sum, 2));
    }

    /* Get the second half of the windowed samples */

    for (i = 0; i < (DCT_LENGTH>>1); i++)
    {
        sum = L_mult(*win_new++, *new_ptr++);
        sum = L_msu(sum, *--win_old, *--old_ptr);
        *out_ptr++ = round16(L_shl(sum, 2));
    }

    /* Save the second half of the new samples for   */
    /* next time, when they will be the old samples. */

    /* pointer arithmetic */

	DSP_memcpy(old_samples, &windowed_data[DCT_LENGTH>>1], DCT_LENGTH>>1);
//
//  new_ptr = windowed_data + (DCT_LENGTH>>1);
//    old_ptr = old_samples;
//    for (i = 0; i < (DCT_LENGTH>>1); i++)
//        *old_ptr++ = *new_ptr++;
}

void coef2sam_init()
{
	DSP_zero(old_samples, DCT_LENGTH);
}

