/*********************************************************************************
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   2004 Polycom, Inc.
**
**	 All rights reserved.
**
*********************************************************************************/

/*********************************************************************************
* Filename: dct_type_iv_a.c
*
* Purpose:  Discrete Cosine Transform, Type IV used for MLT
*
* The basis functions are
*
*	 cos(PI*(t+0.5)*(k+0.5)/block_length)
*
* for time t and basis function number k.  Due to the symmetry of the expression
* in t and k, it is clear that the forward and inverse transforms are the same.
*
*********************************************************************************/

/*********************************************************************************
 Include files
*********************************************************************************/
#include "dct4_a.h"

#include "basop32.h"

extern Uint16 gDct_length_log;

/*********************************************************************************
 Function:    dct_type_iv_a

 Syntax:      void dct_type_iv_a (input, output, dct_length)
                        Int16   input[], output[], dct_length;

 Description: Discrete Cosine Transform, Type IV used for MLT

 Design Notes:

 WMOPS:          |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    1.14      |     1.14
          -------|--------------|----------------
            MAX  |    1.14      |     1.14
          -------|--------------|----------------

           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    2.57      |     2.57       |     2.57
          -------|--------------|----------------|----------------
            MAX  |    2.57      |     2.57       |     2.57
          -------|--------------|----------------|----------------

*********************************************************************************/
extern Int16 buffer_a[], buffer_b[], buffer_c[];

void dct_type_iv_a (Int16 *input, Int16 *output, Int16 dct_length)
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
	Int32   sum, acca;
	Int16   set_span, set_count, set_count_log, pairs_left, sets_left;
	Int16   k, temp;
	cos_msin_t  **table_ptr_ptr, *cos_msin_ptr;

	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/* Do the sum/difference butterflies, the first part of */
	/* converting one N-point transform into N/2 two-point  */
	/* transforms, where N = 1 << DCT_LENGTH_LOG. = 64/128  */
	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	if (dct_length != MAX_DCT_LENGTH)        /* Add bias offsets */
		DSP_add(input, anal_bias, dct_length);

	in_buffer  = input;
	set_span = dct_length;
	set_count = 1;
	for (set_count_log = 0; set_count_log < gDct_length_log - 1; set_count_log++)
	{
		out_buffer = outptr[0];
		outptr[0] = outptr[1];
		outptr[1] = out_buffer;
		/*===========================================================*/
		/* Initialization for the loop over sets at the current size */
		/*===========================================================*/

		in_ptr        = in_buffer;
		next_out_base = out_buffer;

		/*=====================================*/
		/* Loop over all the sets of this size */
		/*=====================================*/

		for (sets_left = set_count; sets_left--;)
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
				acca            = (Int32)in_val_low + (Int32)in_val_high;
				*out_ptr_low++  = (Int16)(acca >> 1);

				acca            = (Int32)in_val_low - (Int32)in_val_high;
				*--out_ptr_high = (Int16)(acca >> 1);
			} while (out_ptr_low < out_ptr_high);

		} /* End of loop over sets of the current size */

		/*============================================================*/
		/* Decide which buffers to use as input and output next time. */
		/* Except for the first time (when the input buffer is the    */
		/* subroutine input) we just alternate the local buffers.     */
		/*============================================================*/

		in_buffer = out_buffer;
		set_span >>= 1;
		set_count <<= 1;
	} /* End of loop over set sizes */

	/*++++++++++++++++++++++++++++++++*/
	/* Do N/2 two-point transforms,   */
	/* where N =  1 << DCT_LENGTH_LOG */
	/*++++++++++++++++++++++++++++++++*/

	pair_ptr = in_buffer;

	buffer_swap = buffer_c;

	for (pairs_left = 1<<(gDct_length_log-1); pairs_left > 0; pairs_left--)
	{
		for (k = 0; k < CORE_SIZE; k++)
			*buffer_swap++ = DSP_mac(pair_ptr, dct_core_a[k], CORE_SIZE);

/*		for (k = 0; k < CORE_SIZE; k++)
		{
			sum=0L;
			for (i = 0; i < CORE_SIZE; i++)
				sum = L_mac(sum, pair_ptr[i], dct_core_a[k][i]);

			*buffer_swap++ = round16(sum);
		}
*/
        /* address arithmetic */
        pair_ptr    += CORE_SIZE;
    }

//	DSP_memcpy(in_buffer, buffer_c, dct_length);
//	for (i = 0; i < dct_length; i++)
//		in_buffer[i] = buffer_c[i];
	in_buffer = buffer_c;
	table_ptr_ptr = a_cos_msin_table;

	/*++++++++++++++++++++++++++++++*/
	/* Perform rotation butterflies */
	/*++++++++++++++++++++++++++++++*/
	out_buffer = outptr[0];
	set_span   = 10;
	for (set_count_log = gDct_length_log -2; set_count_log >= 0; set_count_log--)
	{
		/*===========================================================*/
		/* Initialization for the loop over sets at the current size */
		/*===========================================================*/
		/*    set_span      = 1 << (DCT_LENGTH_LOG - set_count_log); */
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
			temp           = set_span >> 1;

			/* address arithmetic */
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
				/* address arithmetic */
				in_low_even     = *in_ptr_low++;
				in_low_odd      = *in_ptr_low++;
				in_high_even    = *in_ptr_high++;
				in_high_odd     = *in_ptr_high++;
				cosine          = (*cos_msin_ptr).cosine;
				sine            = (*cos_msin_ptr++).minus_sine;

				sum = L_mult(cosine, in_low_even);
				sum = L_msu(sum, sine, in_high_even);
				out_low_even = round16(sum);

				sum = L_mult(sine, in_low_even);
				sum = L_mac(sum, cosine, in_high_even);
				out_high_even = round16(sum);

				cosine          = (*cos_msin_ptr).cosine;
				sine            = (*cos_msin_ptr++).minus_sine;

				sum = L_mult(cosine, in_low_odd);
				sum = L_mac(sum, sine, in_high_odd);
				out_low_odd = round16(sum);

				sum = L_mult(sine, in_low_odd);
				sum = L_msu(sum, cosine, in_high_odd);
				out_high_odd = round16(sum);

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
}

