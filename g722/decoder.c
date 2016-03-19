/***************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   2004 Polycom, Inc.
**
**   All rights reserved.
**
***************************************************************************/

/***************************************************************************
  Filename:    decoder.c

  Purpose:     Contains files used to implement the G.722.1 Annex C decoder
		
  Design Notes:

***************************************************************************/

/***************************************************************************
 Include files
***************************************************************************/
#include "codec.h"
#include "defs.h"
#include "tables.h"
#include "huff_def.h"

#include "basop32.h"

/****************************************************************************************
 Function:    get_next_bit

 Syntax:      void get_next_bit(Bit_Obj *bitobj)

 Description: Returns the next bit in the current word inside the bit object

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |    0.00      |    0.00
          -------|--------------|----------------
            MAX  |    0.00      |    0.00
          -------|--------------|----------------

           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.00      |    0.00        |     0.00
          -------|--------------|----------------|----------------
            MAX  |    0.00      |    0.00        |     0.00
          -------|--------------|----------------|----------------

****************************************************************************************/
void get_next_bit(Bit_Obj *bitobj)
{
    Int16 temp;

    if (bitobj->code_bit_count == 0)
    {
        bitobj->current_word = *bitobj->code_word_ptr++;
        bitobj->code_bit_count = 16;
    }
    bitobj->code_bit_count--;
    temp = bitobj->current_word >> bitobj->code_bit_count;
    bitobj->next_bit = temp & 1;
}

/****************************************************************************************
 Function:    get_rand

 Syntax:      Int16 get_rand()

 Description: Returns a random Int16 based on the seeds inside the rand object

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |    0.00      |    0.00
          -------|--------------|----------------
            MAX  |    0.00      |    0.00
          -------|--------------|----------------

           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.00      |    0.00        |     0.00
          -------|--------------|----------------|----------------
            MAX  |    0.00      |    0.00        |     0.00
          -------|--------------|----------------|----------------

****************************************************************************************/
Int16 get_rand()
{
	/* initialize the random number generator */
	static Rand_Obj randobj = {1, 1, 1, 1};

    Int16 random_word;

    random_word = randobj.seed0 + randobj.seed3;

	if (random_word < 0)
        random_word++;

    randobj.seed3 = randobj.seed2;
    randobj.seed2 = randobj.seed1;
    randobj.seed1 = randobj.seed0;
    randobj.seed0 = random_word;

    return random_word;
}

/***************************************************************************
 Function:    decode_envelope

 Syntax:      void decode_envelope(Bit_Obj *bitobj,
                                   Int16  *decoder_region_standard_deviation,
                                   Int16  *absolute_region_power_index,
                                   Int16  *p_mag_shift)

              inputs:   Bit_Obj *bitobj

              outputs:  Int16  *decoder_region_standard_deviation
                        Int16  *absolute_region_power_index
                        Int16  *p_mag_shift


 Description: Recover differential_region_power_index from code bits

 Design Notes:

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |     0.04     |    0.04
          -------|--------------|----------------
            MAX  |     0.05     |    0.05
          -------|--------------|----------------
				
           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |     0.08     |    0.08        |     0.08
          -------|--------------|----------------|----------------
            MAX  |     0.10     |    0.10        |     0.10
          -------|--------------|----------------|----------------
				
***************************************************************************/
void decode_envelope(Bit_Obj *bitobj,
                     Int16  *decoder_region_standard_deviation,
		             Int16  *absolute_region_power_index,
		             Int16  *p_mag_shift)
{
    Int16 region;
    Int16 i;
    Int16 index = 0;
    Int16 max_index = 0;

    Int16 temp;
    Int32 acca;

    /* get 5 bits from the current code word */
    for (i = 0; i < 5; i++)
    {
        get_next_bit(bitobj);
        index = index << 1;
        index = index + bitobj->next_bit;
    }
    bitobj->number_of_bits_left = bitobj->number_of_bits_left - 5;

    /* ESF_ADJUSTMENT_TO_RMS_INDEX compensates for the current (9/30/96)
        IMLT being scaled to high by the ninth power of sqrt(2). */

    absolute_region_power_index[0] = index - ESF_ADJUSTMENT_TO_RMS_INDEX;
    /* Reconstruct absolute_region_power_index[] from differential_region_power_index[]. */

    i = absolute_region_power_index[0] + REGION_POWER_TABLE_NUM_NEGATIVES;
    if (i > max_index)
        max_index = i;

    temp = int_region_standard_deviation_table[i];
    /* obtain differential_region_power_index */
    for (region = 1; region < NUMBER_OF_REGIONS; region++) 
    {
        index = 0;
        do
        {
            get_next_bit(bitobj);
            i = bitobj->next_bit;
	        index = differential_region_power_decoder_tree[region][index][i];

            bitobj->number_of_bits_left--;
        } while (index > 0);

        acca = (Int32)absolute_region_power_index[region-1] - (Int32)index;
        acca = acca + DRP_DIFF_MIN;
        absolute_region_power_index[region] = (Int16)acca;

    /* DEBUG!!!! - This integer method jointly computes the mag_shift
       and the standard deviations already mag_shift compensated. It
       relies on REGION_POWER_STEPSIZE_DB being exactly 3.010299957 db
       or a square root of 2 chnage in standard deviation. If
       REGION_POWER_STEPSIZE_DB changes, this software must be
       reworked. */

        i = absolute_region_power_index[region] + REGION_POWER_TABLE_NUM_NEGATIVES;
        if (i > max_index)
            max_index = i;

        temp = add(temp, int_region_standard_deviation_table[i]);
    }

    i = 9;

    while ((i >= 0) && ((temp >= 8) || (max_index > 28)))
    {
        i--;
        temp = temp >> 1;
        max_index = max_index - 2;
    }

    *p_mag_shift = i;

    /* pointer arithmetic */
    temp = (Int16 )(REGION_POWER_TABLE_NUM_NEGATIVES + (*p_mag_shift * 2));

    for (region = 0; region < NUMBER_OF_REGIONS; region++) 
    {
        i = absolute_region_power_index[region] + temp;
        decoder_region_standard_deviation[region] = int_region_standard_deviation_table[i];
    }
}

/***************************************************************************
 Function:     rate_adjust_categories

 Syntax:       void rate_adjust_categories(Int16 categorization_control,
                                           Int16 *decoder_power_categories,
                                           Int16 *decoder_category_balances)

               inputs:    Int16 categorization_control,
                          Int16 *decoder_power_categories,
                          Int16 *decoder_category_balances

               outputs:   Int16 categorization_control,
                          Int16 *decoder_power_categories,

 Description:  Adjust the power categories based on the categorization control

 Design Notes:

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |    0.00      |    0.00
          -------|--------------|----------------
            MAX  |    0.00      |    0.00
          -------|--------------|----------------
				
           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.00      |    0.00        |     0.00
          -------|--------------|----------------|----------------
            MAX  |    0.01      |    0.01        |     0.01
          -------|--------------|----------------|----------------
				
***************************************************************************/
void rate_adjust_categories(Int16 categorization_control,
			                Int16 *decoder_power_categories,
			                Int16 *decoder_category_balances)
{
    Int16 i;
    Int16 region;

	for(i = 0; i < categorization_control; i++)
    {
        region = decoder_category_balances[i];
        decoder_power_categories[region]++;
    }
}

/****************************************************************************************
 Function:    index_to_array

 Syntax:      number_of_non_zero =
                 index_to_array(Int16 index, 
                                Int16 array[MAX_VECTOR_DIMENSION],
                                Int16 category)

                inputs:  Int16 index
                         Int16 category                     
                       
                outputs: Int16 array[MAX_VECTOR_DIMENSION]
                         -- used in decoder to access mlt_quant_centroid table

                         Int16 number_of_non_zero
                         -- number of non zero elements in the array
 
 Description: Computes an array of sign bits with the length of the category
              vector. Returns the number of sign bits and the array

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |     0.00     |     0.00
          -------|--------------|----------------
            MAX  |     0.00     |     0.00
          -------|--------------|----------------

           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |     0.00     |     0.00       |      0.00
          -------|--------------|----------------|----------------
            MAX  |     0.00     |     0.00       |      0.00
          -------|--------------|----------------|----------------

****************************************************************************************/
Int16 index_to_array(Int16 index,Int16 *array,Int16 category)
{
    Int16 j, q, p;
    Int16 number_of_non_zero;
    Int16 max_bin_plus_one;
    Int16 inverse_of_max_bin_plus_one;
    Int16 temp;

    number_of_non_zero = 0;

    p = index;

    max_bin_plus_one = max_bin[category] + 1;
    inverse_of_max_bin_plus_one = max_bin_plus_one_inverse[category];

    for (j = vector_dimension[category] - 1; j >= 0; j--) 
    {
        q = mult(p, inverse_of_max_bin_plus_one);
        temp = (Int16)((Int32)q * (Int32)max_bin_plus_one);
        array[j] = sub(p, temp);

        p = q;

        if (array[j] != 0)
            number_of_non_zero++;
    }
    return(number_of_non_zero);
}

/***************************************************************************
 Function:    decode_vector_quantized_mlt_indices

 Syntax:      void decode_vector_quantized_mlt_indices(Bit_Obj  *bitobj,
                                                       Int16   *decoder_region_standard_deviation,
                                                       Int16   *decoder_power_categories,
                                                       Int16   *decoder_mlt_coefs)
              inputs:    Bit_Obj  *bitobj
                         Int16   *decoder_region_standard_deviation
                         Int16   *decoder_power_categories

              outputs:   Int16   *decoder_mlt_coefs


 Description: Decode MLT coefficients

 Design Notes:

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |    0.60      |    0.72
          -------|--------------|----------------
            MAX  |    0.67      |    0.76
          -------|--------------|----------------
				
           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.77      |    0.98        |     1.28
          -------|--------------|----------------|----------------
            MAX  |    1.05      |    1.18        |     1.36
          -------|--------------|----------------|----------------
				
***************************************************************************/
void decode_vector_quantized_mlt_indices(Bit_Obj  *bitobj,
                                         Int16   *decoder_region_standard_deviation,
					                     Int16   *decoder_power_categories,
					                     Int16   *decoder_mlt_coefs)
{
    Int16 standard_deviation;
    Int16 *decoder_mlt_ptr;
    Int16 *raw_mlt_ptr;
    Int16 decoder_mlt_value;
    Int16 noifill[2];
    Int16 noise_fill_factor[3] = {5793,8192,23170};
    Int16 region;
    Int16 category;
    Int16 j, n;
    Int16 k[MAX_VECTOR_DIMENSION];
    Int16 vec_dim;
    Int16 num_vecs;
    Int16 index;
    Int16 signs_index;
    Int16 bit;
    Int16 num_sign_bits;
    Int16 ran_out_of_bits_flag = 0;
    Int16 *decoder_table_ptr;
    Int16 random_word;

    Int16 temp;
    Int32 acca;

	raw_mlt_ptr = decoder_mlt_coefs;
    for (region = 0; region < NUMBER_OF_REGIONS; region++) 
    {
        category = decoder_power_categories[region];
        decoder_mlt_ptr = raw_mlt_ptr;
        standard_deviation = decoder_region_standard_deviation[region];

        if (category < 7)
        {
            /* Get the proper table of decoder tables, vec_dim, and num_vecs for the cat */
            decoder_table_ptr = (Int16 *) table_of_decoder_tables[category];
            vec_dim = vector_dimension[category];
            num_vecs = number_of_vectors[category];

            for (n = 0; n < num_vecs; n++)
            {
                index = 0;

                /* get index */
                do
                {
                    if (bitobj->number_of_bits_left <= 0) 
                    {
                        ran_out_of_bits_flag = 1;
    	                break;
    	            }

    	            get_next_bit(bitobj);

                    temp = index << 1;
                    index = decoder_table_ptr[temp + bitobj->next_bit];

	                bitobj->number_of_bits_left--;
                } while (index > 0);

                if (ran_out_of_bits_flag != 0)
	                break;

                index = negate(index);

                /* convert index into array used to access the centroid table */
                /* get the number of sign bits in the index */
                num_sign_bits = index_to_array(index, k, category);

	            if (bitobj->number_of_bits_left >= num_sign_bits)
                {
                    if (num_sign_bits != 0) 
                    {
	                    signs_index = 0;
                        for (j = 0; j < num_sign_bits; j++) 
                        {
	                        get_next_bit(bitobj);
       	                    signs_index <<= 1;
		                    signs_index += bitobj->next_bit;
                            bitobj->number_of_bits_left--;
                        }
	                    temp = num_sign_bits - 1;
                        bit = 1 << temp;
	                }

                    for (j = 0; j < vec_dim; j++)
                    {
	                    acca = (Int32)standard_deviation * (Int32)mlt_quant_centroid[category][k[j]];
                        acca = acca >> 12;
                        decoder_mlt_value = (Int16)acca;

                        if (decoder_mlt_value != 0)
                        {
                            if ((signs_index & bit) == 0)
		                        decoder_mlt_value = negate(decoder_mlt_value);
		                    bit = bit >> 1;
	                    }
                        *decoder_mlt_ptr++ = decoder_mlt_value;
	                }
	            }
	            else
                {
	                ran_out_of_bits_flag = 1;
	                break;
	            }
	        }
            /* If ran out of bits during decoding do noise fill for remaining regions. */
            /* DEBUG!! - For now also redo all of last region with all noise fill. */
            if (ran_out_of_bits_flag != 0)
            {
        	    temp = add(region, 1);
                for (j = temp; j < NUMBER_OF_REGIONS; j++)
                {
                    decoder_power_categories[j] = 7;
                }
        	    category = 7;
        	}
        }

        decoder_mlt_ptr = raw_mlt_ptr;
        noifill[1] = mult(standard_deviation, noise_fill_factor[category - 5]);
        noifill[0] = negate(noifill[1]);

        if ((category == 5) || (category == 6))
        {
	        random_word = get_rand();
	        for (j = 0; j < 10; j++)
            {
                if (*decoder_mlt_ptr == 0)
                {
                    temp = random_word & 1;
                    *decoder_mlt_ptr = noifill[temp];
                    random_word >>= 1;
                }
	            /* pointer arithmetic */
                decoder_mlt_ptr++;
	        }
	        random_word = get_rand();
	        for (j = 0; j < 10; j++)
            {
                if (*decoder_mlt_ptr == 0)
                {
                    temp = random_word & 1;
                    *decoder_mlt_ptr = noifill[temp];
                    random_word >>= 1;
                }
	            /* pointer arithmetic */
                decoder_mlt_ptr++;
	        }
        }

        /* if (category == 7) */
        if (category == 7)
        {
            random_word = get_rand();
            for (j = 0; j < 10; j++)
            {
                temp = random_word & 1;
                *decoder_mlt_ptr++ = noifill[temp];
                random_word >>= 1;
            }
            random_word = get_rand();
            for (j = 0; j < 10; j++)
            {
                temp = random_word & 1;
                *decoder_mlt_ptr++ = noifill[temp];
                random_word >>= 1;
            }
        }
		raw_mlt_ptr += REGION_SIZE;
    }

    bitobj->number_of_bits_left -= ran_out_of_bits_flag;
}

/***************************************************************************
 Function:     test_4_frame_errors

 Syntax:       void test_4_frame_errors(Bit_Obj *bitobj,
                                        Int16 num_categorization_control_possibilities,
                                        Int16 *frame_error_flag,
                                        Int16 categorization_control,
                                        Int16 *absolute_region_power_index)

               inputs:   bit_obj
                         num_categorization_control_possibilities
                         frame_error_flag
                         categorization_control
                         absolute_region_power_index

               outputs:  frame_error_flag

 Description:  Tests for error conditions and sets the frame_error_flag accordingly

 Design Notes:

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |    0.01      |     0.01
          -------|--------------|----------------
            MAX  |    0.04      |     0.08
          -------|--------------|----------------
				
           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.01      |     0.01       |      0.01
          -------|--------------|----------------|----------------
            MAX  |    0.02      |     0.06       |      0.08
          -------|--------------|----------------|----------------
				
***************************************************************************/
void test_4_frame_errors(Bit_Obj *bitobj,
                         Int16 *frame_error_flag,
                         Int16 categorization_control,
                         Int16 *absolute_region_power_index)
{
    Int16 region;
    Int16 i;
    Int32 acca;
    Int32 accb;

    /* Test for bit stream errors. */

    if (bitobj->number_of_bits_left > 0)
    {
        for (i = 0; i < bitobj->number_of_bits_left; i++)
        {
            get_next_bit(bitobj);
            if (bitobj->next_bit == 0)
            {
                *frame_error_flag = 1;
            }
        }	
    }
    else
    {
        if (categorization_control < NUM_CATEGORIZATION_CONTROL_POSSIBILITIES - 1)
        {
            if (bitobj->number_of_bits_left < 0)
            {
                *frame_error_flag |= 2;
            }
        }
    }

    /* checks to ensure that abs_region_power_index is within range */
    /* the error flag is set if it is out of range */
    for (region = 0; region < NUMBER_OF_REGIONS; region++)
    {
        /*  the next two lines of comments were modified in release 1.2
	   *  to correct the description of the range of
	   *  absolute_region_power_index[] to be tested in the next
	   *  9 lines of code.
	   */
	  /*  if ((absolute_region_power_index[region] > 31) ||
            (absolute_region_power_index[region] < -8) */

        acca = (Int32)absolute_region_power_index[region] + ESF_ADJUSTMENT_TO_RMS_INDEX;
        accb = acca - 31;
        acca = acca + 8;

        /* the next line was modifed in release 1.2 to
	   * correct miss typed code and error checking.
	   */
        if ((accb > 0) || (acca < 0))
        {
            *frame_error_flag |= 4;
        }
    }
}

/***************************************************************************
 Function:    error_handling

 Syntax:      void error_handling(Int16 number_of_coefs,
                                  Int16 number_of_valid_coefs,
                                  Int16 *frame_error_flag,
                                  Int16 *decoder_mlt_coefs,
                                  Int16 *p_mag_shift,
                                  Int16 *p_old_mag_shift)

              inputs:  number_of_coefs
                       number_of_valid_coefs
                       frame_error_flag
                       p_old_mag_shift


              outputs: decoder_mlt_coefs
                       p_mag_shift
                       p_old_mag_shift



 Description: If both the current and previous frames are errored,
              set the mlt coefficients to 0. If only the current frame
              is errored, then repeat the previous frame's mlt coefficients.

 Design Notes:

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|----------------
            AVG  |    0.02      |     0.02
          -------|--------------|----------------
            MAX  |    0.03      |     0.03
          -------|--------------|----------------
				
           14kHz |    24kbit    |    32kbit      |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.03      |     0.03       |     0.03
          -------|--------------|----------------|----------------
            MAX  |    0.03      |     0.03       |     0.06
          -------|--------------|----------------|----------------
				
***************************************************************************/
void error_handling(Int16 number_of_coefs,
                    Int16 number_of_valid_coefs,
                    Int16 frame_error_flag,
                    Int16 *decoder_mlt_coefs,
                    Int16 *p_mag_shift,
                    Int16 *p_old_mag_shift)
{
    Int16 i;
	static Int16 old_decoder_mlt_coefs[DCT_LENGTH];

    if (frame_error_flag) 
    {
        for (i = 0; i < number_of_valid_coefs; i++)
        {
            decoder_mlt_coefs[i] = old_decoder_mlt_coefs[i];
            old_decoder_mlt_coefs[i] = 0;
        }
        
        *p_mag_shift = *p_old_mag_shift;
        *p_old_mag_shift = 0;
    }
    else 
    {
        /* Store in case next frame is error. */
        for (i = 0; i < number_of_valid_coefs; i++)
        {
            old_decoder_mlt_coefs[i] = decoder_mlt_coefs[i];
        }

        *p_old_mag_shift = *p_mag_shift;
    }

    /* Zero out the upper 1/8 of the spectrum. */
    for (i = number_of_valid_coefs; i < number_of_coefs; i++)
    {
        decoder_mlt_coefs[i] = 0;
    }
}

/***************************************************************************
 Function:    decoder

 Syntax:      void decoder(Bit_Obj *bitobj,
                           Int16 number_of_regions,
                           Int16 *decoder_mlt_coefs,
                           Int16 *p_mag_shift,
                           Int16 *p_old_mag_shift)

              inputs:    Bit_Obj *bitobj
                         Int16 number_of_regions
                         Int16 *p_old_mag_shift

              outputs:   Int16 *decoder_mlt_coefs,
                         Int16 *p_mag_shift,



 Description: Decodes the out_words into mlt coefs using G.722.1 Annex C

 Design Notes:

 WMOPS:     7kHz |   24kbit    |    32kbit
          -------|-------------|----------------
            AVG  |    0.84     |    0.94
          -------|-------------|----------------
            MAX  |    0.90     |    1.00
          -------|-------------|----------------
				
           14kHz |   24kbit    |    32kbit      |     48kbit
          -------|-------------|----------------|----------------
            AVG  |    1.31     |    1.56        |     1.88
          -------|-------------|----------------|----------------
            MAX  |    1.59     |    1.80        |     1.98
          -------|-------------|----------------|----------------
				
***************************************************************************/
void decoder(Int16 *out_words,
	         Int16 *decoder_mlt_coefs,
	         Int16 *p_mag_shift,
             Int16 *p_old_mag_shift)
{
    Int16  absolute_region_power_index[NUMBER_OF_REGIONS];
    Int16  decoder_power_categories[NUMBER_OF_REGIONS];
    Int16  decoder_category_balances[NUM_CATEGORIZATION_CONTROL_POSSIBILITIES-1];
    Uint16 categorization_control;
    Int16  decoder_region_standard_deviation[NUMBER_OF_REGIONS];
    Int16  i;

    Int16  number_of_coefs;
    Int16  number_of_valid_coefs;
	Int16  frame_error_flag = 0;
	Bit_Obj bitobj;

	number_of_coefs = DCT_LENGTH;
	number_of_valid_coefs = NUMBER_OF_VALID_COEFS;

	// reinit the current word to point to the start of the buffer
	bitobj.code_word_ptr = out_words;
	bitobj.code_bit_count = 0;
	bitobj.number_of_bits_left = (Uint16)(BITRATE * 20);    // BITRATE in kbps

    /* convert the bits to absolute region power index and decoder_region_standard_deviation */
    decode_envelope(&bitobj,
                    decoder_region_standard_deviation,
                    absolute_region_power_index,
                    p_mag_shift);

    /* fill the categorization_control with NUM_CATEGORIZATION_CONTROL_BITS */
    categorization_control = 0;
    for (i = 0; i < NUM_CATEGORIZATION_CONTROL_BITS; i++) 
    {
        get_next_bit(&bitobj);
        categorization_control <<= 1;
        categorization_control += bitobj.next_bit;
    }

    bitobj.number_of_bits_left -= NUM_CATEGORIZATION_CONTROL_BITS;

    /* obtain decoder power categories and category balances */
    /* based on the absolute region power index              */
    categorize(bitobj.number_of_bits_left,
               absolute_region_power_index,
               decoder_power_categories,
               decoder_category_balances);

    /* perform adjustmaents to the power categories and category balances based on the cat control */
    rate_adjust_categories(categorization_control,
		                   decoder_power_categories,
		                   decoder_category_balances);

    /* decode the quantized bits into mlt coefs */
    decode_vector_quantized_mlt_indices(&bitobj,
                                        decoder_region_standard_deviation,
				                        decoder_power_categories,
				                        decoder_mlt_coefs);

    /* test for frame errors */
    test_4_frame_errors(&bitobj,
                        &frame_error_flag,
                        categorization_control,
                        absolute_region_power_index);

    /* perform error handling operations */
    error_handling(number_of_coefs,
                   number_of_valid_coefs,
                   frame_error_flag,
                   decoder_mlt_coefs,
                   p_mag_shift,
                   p_old_mag_shift);
}

