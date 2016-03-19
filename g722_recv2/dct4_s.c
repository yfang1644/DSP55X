/********************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   2004 Polycom, Inc.
**
**   All rights reserved.
**
********************************************************************************/

/********************************************************************************
* Filename: dct_type_iv_s.c
*
* Purpose:  Discrete Cosine Transform, Type IV used for inverse MLT
*
* The basis functions are
*
*	 cos(PI*(t+0.5)*(k+0.5)/block_length)
*
* for time t and basis function number k.  Due to the symmetry of the expression
* in t and k, it is clear that the forward and inverse transforms are the same.
*
*********************************************************************************/

/***************************************************************************
 Include files
***************************************************************************/
#include "dct4_s.h"

#include "basop32.h"

extern Uint16 gDct_length_log;
/********************************************************************************
 Function:    dct_type_iv_s

 Syntax:      void dct_type_iv_s (Int16 *input,Int16 *output,Int16 dct_length)

 Description: Discrete Cosine Transform, Type IV used for inverse MLT

 Design Notes:

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |     1.74     |     1.74
          -------|--------------|----------------
            MAX  |     1.74     |     1.74
          -------|--------------|----------------
				
           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |     3.62     |     3.62       |      3.62
          -------|--------------|----------------|----------------
            MAX  |     3.62     |     3.62       |      3.62
          -------|--------------|----------------|----------------

********************************************************************************/
extern Int16 buffer_a[], buffer_b[], buffer_c[];

void dct_type_iv_s (Int16 *input, Int16 *output,Int16 dct_length)
{
    Int16   *in_ptr, *in_ptr_low, *in_ptr_high, *next_in_base;
    Int16   *out_ptr_low, *out_ptr_high, *next_out_base;
    Int16   *out_buffer, *in_buffer, *buffer_swap;
    Int16   *outptr[2] = {buffer_a, buffer_b};
    Int16   in_val_low, in_val_high;
    Int16   in_low_even, in_low_odd;
    Int16   in_high_even, in_high_odd;
    Int16   out_low_even, out_low_odd;
    Int16   out_high_even, out_high_odd;
    Int16   *pair_ptr;
    Int16   cosine, sine;
    Int16   set_span, set_count, set_count_log, pairs_left, sets_left;
    Int16   k, temp;
    Int32   sum, acca;
    cos_msin_t	**table_ptr_ptr, *cos_msin_ptr;

    Int16   *dither_ptr;

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Do the sum/difference butterflies, the first part of            */
    /* converting one N-point transform into 32 - 10 point transforms  */
    /* transforms, where N = 1 << DCT_LENGTH_LOG.                      */
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	if(dct_length != MAX_DCT_LENGTH)
	    dither_ptr = dither;
	else
	    dither_ptr = max_dither;

    in_buffer  = input;
    out_buffer = outptr[0];

	in_ptr         = in_buffer;
	next_out_base  = out_buffer;
	/*||||||||||||||||||||||||||||||||||||||||||||*/
	/* Set up output pointers for the current set */
	/*||||||||||||||||||||||||||||||||||||||||||||*/
	/* pointer arithmetic */
	out_ptr_low    = next_out_base;
	next_out_base += dct_length;
	out_ptr_high   = next_out_base;

	/*||||||||||||||||||||||||||||||||||||||||||||||||||*/
	/* Loop over all the butterflies in the current set */
	/*||||||||||||||||||||||||||||||||||||||||||||||||||*/

	do
	{
		in_val_low      = *in_ptr++;
		in_val_high     = *in_ptr++;

		/* BEST METHOD OF GETTING RID OF BIAS, BUT COMPUTATIONALLY UNPLEASANT */
		/* ALTERNATIVE METHOD, SMEARS BIAS OVER THE ENTIRE FRAME, COMPUTATIONALLY SIMPLEST. */
		/* IF THIS WORKS, IT'S PREFERABLE */

		acca = (Int32)in_val_low + (Int32)(*dither_ptr++);
		acca = acca + (Int32)in_val_high;
		*out_ptr_low++ = (Int16)(acca >> 1);

		acca = (Int32)in_val_low + (Int32)(*dither_ptr++);
		acca = acca - (Int32)in_val_high;
		*--out_ptr_high = (Int16)(acca >> 1);

		/* this involves comparison of pointers */
		/* pointer arithmetic */

	} while (out_ptr_low < out_ptr_high);

	/*============================================================*/
	/* Decide which buffers to use as input and output next time. */
	/* Except for the first time (when the input buffer is the    */
	/* subroutine input) we just alternate the local buffers.     */
	/*============================================================*/

	in_buffer = out_buffer;

	/*==============================================================*/
	/* Loop over all(except set_count_log==0) the sets of this size */
	/*==============================================================*/
	set_span       = dct_length;
    for (set_count_log = 1; set_count_log < gDct_length_log - 1; set_count_log++)
    {
        outptr[0] = outptr[1];
        outptr[1] = out_buffer;
        out_buffer = outptr[0];
        /*===========================================================*/
        /* Initialization for the loop over sets at the current size */
        /*===========================================================*/

        /*    set_span      = 1 << (DCT_LENGTH_LOG - set_count_log); */
        set_span    >>= 1;
        set_count     = 1 << set_count_log;
        in_ptr        = in_buffer;
        next_out_base = out_buffer;

		for (sets_left = set_count; sets_left--; )
		{
			/*||||||||||||||||||||||||||||||||||||||||||||*/
			/* Set up output pointers for the current set */
			/*||||||||||||||||||||||||||||||||||||||||||||*/

			out_ptr_low    = next_out_base;
			next_out_base += set_span;
			out_ptr_high   = next_out_base;

			/*||||||||||||||||||||||||||||||||||||||||||||||||||*/
			/* Loop over all the butterflies in the current set */
			/*||||||||||||||||||||||||||||||||||||||||||||||||||*/

			do
			{
				in_val_low      = *in_ptr++;
				in_val_high     = *in_ptr++;

				*out_ptr_low++  = add(in_val_low, in_val_high);
				*--out_ptr_high = sub(in_val_low, in_val_high);
			} while (out_ptr_low < out_ptr_high);
		} /* End of loop over sets of the current size */

        /*============================================================*/
        /* Decide which buffers to use as input and output next time. */
        /* Except for the first time (when the input buffer is the    */
        /* subroutine input) we just alternate the local buffers.     */
        /*============================================================*/

        in_buffer = out_buffer;

    } /* End of loop over set sizes */
    out_buffer = outptr[1];

    /*++++++++++++++++++++++++++++++++*/
    /* Do 32 - 10 point transforms    */
    /*++++++++++++++++++++++++++++++++*/

    pair_ptr = in_buffer;
    buffer_swap = buffer_c;

	for (pairs_left = 1 << (gDct_length_log - 1); pairs_left > 0; pairs_left--)
	{
		for (k = 0; k < CORE_SIZE; k++)
			*buffer_swap++ = DSP_mac(pair_ptr, dct_core_s[k], CORE_SIZE);

/*		for (k = 0; k < CORE_SIZE; k++)
		{
			sum = 0L;
			for (i = 0; i < CORE_SIZE; i++)
				sum = L_mac(sum, pair_ptr[i], dct_core_s[k][i]);

			*buffer_swap++ = round16(sum);
		}
*/
		pair_ptr    += CORE_SIZE;
	}

	DSP_memcpy(in_buffer, buffer_c, dct_length>>1);
//	for (i = 0;i < dct_length; i++)
//		in_buffer[i] = buffer_c[i];

    table_ptr_ptr = s_cos_msin_table;

    /*++++++++++++++++++++++++++++++*/
    /* Perform rotation butterflies */
    /*++++++++++++++++++++++++++++++*/
    set_span = 10;
    for (set_count_log = gDct_length_log - 2; set_count_log >= 0; set_count_log--)
    {
        /*===========================================================*/
        /* Initialization for the loop over sets at the current size */
        /*===========================================================*/

        set_span    <<= 1;
        set_count     = 1 << set_count_log;
        next_in_base  = in_buffer;
        next_out_base = (set_count_log) ? out_buffer : output;

        /*=====================================*/
        /* Loop over all the sets of this size */
        /*=====================================*/

        for (sets_left = set_count; sets_left--; )
        {
            /*|||||||||||||||||||||||||||||||||||||||||*/
            /* Set up the pointers for the current set */
            /*|||||||||||||||||||||||||||||||||||||||||*/

            in_ptr_low     = next_in_base;

            temp = set_span >> 1;
            in_ptr_high    = in_ptr_low + temp;

            next_in_base  += set_span;

            out_ptr_low    = next_out_base;

            next_out_base += set_span;
            out_ptr_high   = next_out_base;

            cos_msin_ptr   = *table_ptr_ptr;

            /*||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
            /* Loop over all the butterfly pairs in the current set */
            /*||||||||||||||||||||||||||||||||||||||||||||||||||||||*/

            do
            {
                in_low_even     = *in_ptr_low++;
                in_low_odd      = *in_ptr_low++;
                in_high_even    = *in_ptr_high++;
                in_high_odd     = *in_ptr_high++;
                cosine          = (*cos_msin_ptr).cosine;
                sine            = (*cos_msin_ptr++).minus_sine;

                sum = L_mult(cosine, in_low_even);
                sum = L_msu(sum, sine, in_high_even);
                out_low_even = round16(L_shl(sum, 1));

                sum = L_mult(sine, in_low_even);
                sum = L_mac(sum, cosine, in_high_even);
                out_high_even = round16(L_shl(sum, 1));

                cosine          = (*cos_msin_ptr).cosine;
                sine            = (*cos_msin_ptr++).minus_sine;

                sum = L_mult(cosine, in_low_odd);
                sum = L_mac(sum, sine, in_high_odd);
                out_low_odd = round16(L_shl(sum, 1));

                sum = L_mult(sine, in_low_odd);
                sum = L_msu(sum, cosine, in_high_odd);
                out_high_odd = round16(L_shl(sum, 1));

                *out_ptr_low++  = out_low_even;
                *--out_ptr_high = out_high_even;
                *out_ptr_low++  = out_low_odd;
                *--out_ptr_high = out_high_odd;
            } while (out_ptr_low < out_ptr_high);

        } /* End of loop over sets of the current size */

        /*=============================================*/
        /* Swap input and output buffers for next time */
        /*=============================================*/

        buffer_swap = in_buffer;
        in_buffer   = out_buffer;
        out_buffer  = buffer_swap;

        table_ptr_ptr++;
    }

    if (dct_length != MAX_DCT_LENGTH)    /*  ADD IN BIAS FOR OUTPUT */
    	DSP_add(output, syn_bias_7khz, dct_length);
}

