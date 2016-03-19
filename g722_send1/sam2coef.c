/******************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
******************************************************************************/

/******************************************************************************
* Filename: samples_to_rmlt_coefs.c
*
* Purpose:  Convert Samples to Reversed MLT (Modulated Lapped Transform) 
*           Coefficients
*
*     The "Reversed MLT" is an overlapped block transform which uses
*     even symmetry * on the left, odd symmetry on the right and a
*     Type IV DCT as the block transform.  * It is thus similar to a
*     MLT which uses odd symmetry on the left, even symmetry * on the
*     right and a Type IV DST as the block transform.  In fact, it is
*     equivalent * to reversing the order of the samples, performing
*     an MLT and then negating all * the even-numbered coefficients.
*
******************************************************************************/

/***************************************************************************
 Include files
***************************************************************************/
#include "codec.h"
#include "defs.h"
#include "tables.h"

#include "basop32.h"

/***************************************************************************
 Function:    samples_to_rmlt_coefs

 Syntax:      Int16 samples_to_rmlt_coefs(new_samples,
                                           old_samples,
                                           coefs)
                    Int16 *new_samples;
                    Int16 *old_samples;
                    Int16 *coefs;

 Description: Convert samples to MLT coefficients

 Design Notes:

 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    1.40      |     1.40
          -------|--------------|----------------
            MAX  |    1.40      |     1.40
          -------|--------------|----------------
				
           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    3.07      |     3.07       |     3.07
          -------|--------------|----------------|----------------
            MAX  |    3.10      |     3.10       |     3.10
          -------|--------------|----------------|----------------
				
***************************************************************************/

extern Int16 windowed_data[];
#pragma DATA_ALIGN (old_samples, 2)
static Int16 old_samples[DCT_LENGTH*2];	// left and right channel
Int16 samples_to_rmlt_coefs(Int16 *new_samples, Int16 *coefs, Uint16 chn)
{
    Int16	i, mag_shift;
    Int16	*sam_low, *sam_high;
    Int16	*win_low, *win_high;
    Int16	*dst_ptr;

    Int32	acca;
    Int32	sum;
    Int16	temp;
    Int16	temp_max;

//	static Int16 old_samples[DCT_LENGTH];

    /*++++++++++++++++++++++++++++++++++++++++++++*/
    /* Get the first half of the windowed samples */
    /*++++++++++++++++++++++++++++++++++++++++++++*/

    dst_ptr  = windowed_data;

    /* address arithmetic */
    win_high = samples_to_rmlt_window + (DCT_LENGTH >> 1);
    win_low = win_high;

    /* address arithmetic */
    sam_high = &old_samples[chn*DCT_LENGTH + (DCT_LENGTH>>1)];

    sam_low  = sam_high;

    for (i = 0; i < (DCT_LENGTH>>1); i++)
    {
        acca = L_mult(*--win_low, *--sam_low);
        acca = L_mac(acca, *win_high++, *sam_high++);
        *dst_ptr++ = round16(acca);
    }

    /*+++++++++++++++++++++++++++++++++++++++++++++*/
    /* Get the second half of the windowed samples */
    /*+++++++++++++++++++++++++++++++++++++++++++++*/

    sam_low  = new_samples;

    /* address arithmetic */
    sam_high = new_samples + DCT_LENGTH;

    for (i = 0; i < (DCT_LENGTH>>1); i++)
    {
        acca = L_mult(*--win_high, *sam_low++);
        acca = L_msu(acca, *win_low++, *--sam_high);
        *dst_ptr++ = round16(acca);
    }

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Save the new samples for next time, when they will be the old samples */
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	DSP_memcpy(&old_samples[chn*DCT_LENGTH], new_samples, DCT_LENGTH>>1);
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Calculate how many bits to shift up the input to the DCT.             */
    /* calculate power of windowed_data frame                                */
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    temp_max = 0;
	sum = 0L;

    for(i = 0; i < DCT_LENGTH; i++)
    {
        temp = abs_s(windowed_data[i]);
        sum = sum + (Int32)temp;
        if(temp > temp_max)
            temp_max = temp;
    }

    mag_shift = 0;

    if (temp_max < 14000)
    {
		temp = temp_max;
        if(temp_max < 438)
            temp = temp_max + 1;
        acca = (Int32)temp * 9587;
        temp = (Int16)(acca >> 19);
        temp = norm_s(temp);
        if (temp == 0)
            mag_shift = 9;
        else
            mag_shift = temp - 6;        
    }

    acca = sum >> 7;

    if (temp_max < acca)
        mag_shift = mag_shift - 1;

    for(i = 0; i < DCT_LENGTH; i++)
        windowed_data[i] = shl(windowed_data[i], mag_shift);

    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Perform a Type IV DCT on the windowed data to get the coefficients */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    dct_type_iv_a(windowed_data, coefs, DCT_LENGTH);

    return(mag_shift);
}

void sam2coef_init()
{
	DSP_zero(old_samples, DCT_LENGTH);
}

