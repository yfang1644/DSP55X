/***************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
***************************************************************************/

/***************************************************************************
  Filename:    encoder.c

  Purpose:     Contains files used to implement the G.722.1 Annex C encoder

  Design Notes:

***************************************************************************/

/***************************************************************************
 Include files
***************************************************************************/

#include "codec.h"
#include "defs.h"
#include "huff_def.h"
#include "tables.h"

#include "basop32.h"

/***************************************************************************
 Function:    bits_to_words

 Syntax:      bits_to_words(UInt32 *region_mlt_bits,
                            Int16  *region_mlt_bit_counts,
                            Int16  *drp_num_bits,
                            UInt16 *drp_code_bits,
                            Int16  *out_words,
                            Int16  categorization_control,
                            Int16  num_categorization_control_bits,
                            Int16  number_of_bits_per_frame)


 Description: Stuffs the bits into words for output

 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|---------------
            AVG  |    0.09      |    0.12
          -------|--------------|---------------
            MAX  |    0.10      |    0.13
          -------|--------------|---------------

           14kHz |    24kbit    |    32kbit     |     48kbit
          -------|--------------|---------------|----------------
            AVG  |    0.12      |    0.15       |     0.19
          -------|--------------|---------------|----------------
            MAX  |    0.14      |    0.17       |     0.21
          -------|--------------|---------------|----------------

***************************************************************************/
void bits_to_words(Uint32 *region_mlt_bits,
                   Int16  *region_mlt_bit_counts,
                   Int16  *drp_num_bits,
                   Uint16 *drp_code_bits,
                   Int16  *out_words,
                   Int16  categorization_control,
                   Int16  number_of_bits_per_frame)
{
    Int16  out_word_index = 0;
    Int16  j;
    Int16  region;
    Int16  out_word;
    Int16  region_bit_count;
    Int16  current_word_bits_left;
    Int16  slice;
    Int16  out_word_bits_free = 16;
    Uint32 *in_word_ptr;
    Uint32 current_word;

    Int32  acca;
    Int32  accb;
    Int16  temp;

    /* First set up the categorization control bits to look like one more set of region power bits. */
    out_word = 0;

    drp_num_bits[NUMBER_OF_REGIONS] = NUM_CATEGORIZATION_CONTROL_BITS;

    drp_code_bits[NUMBER_OF_REGIONS] = (Uint16)categorization_control;

    /* These code bits are right justified. */
    for (region = 0; region <= NUMBER_OF_REGIONS; region++)
    {
        current_word_bits_left = drp_num_bits[region];

        current_word = (Uint32)drp_code_bits[region];

        j = sub(out_word_bits_free, current_word_bits_left);

        if (j <= 0)
        {
            out_word = out_word + (current_word >> (-j));
            out_words[out_word_index++] = out_word;
            out_word_bits_free = 16 + j;
            out_word = current_word << out_word_bits_free;
        }
        else
        {
            out_word_bits_free = j;
            out_word = out_word + (current_word << j);
        }
    }

    /* These code bits are left justified. */

    for (region = 0; region < NUMBER_OF_REGIONS; region++)
    {
        accb = (Int32)out_word_index << 4;
        accb = accb - (Int32)number_of_bits_per_frame;
        if(accb < 0)
        {
            temp = shl(region, 2);
            in_word_ptr = &region_mlt_bits[temp];
            region_bit_count = region_mlt_bit_counts[region];

            if(region_bit_count < 32)
                current_word_bits_left = region_bit_count;
            else
                current_word_bits_left = 32;

            current_word = *in_word_ptr++;

            acca = (Int32)out_word_index << 4;
            acca = acca - (Int32)number_of_bits_per_frame;

            /* from while loop */
            while ((region_bit_count > 0) && (acca < 0))
            {
                /* from while loop */
                j = sub(out_word_bits_free, current_word_bits_left);
                if (j > 0)
                {
                    temp = 32 - current_word_bits_left;
                    slice = (Int16) (current_word >> temp);

                    out_word = out_word + (slice << j);
                    out_word_bits_free = j;

                    current_word_bits_left = 0;
                }
                else
                {
                    temp = 32 - out_word_bits_free;
                    slice = (Int16) (current_word >> temp);

                    out_word = add(out_word, slice);
                    current_word <<= out_word_bits_free;
                    current_word_bits_left = -j;
                    out_words[out_word_index++] = out_word;

                    out_word = 0;
                    out_word_bits_free = 16;
                }

                if (current_word_bits_left == 0)
                {
                    current_word = *in_word_ptr++;
                    region_bit_count = sub(region_bit_count, 32);

                    /* current_word_bits_left = MIN(32,region_bit_count); */
                    if(region_bit_count < 32)
                        current_word_bits_left = region_bit_count;
                    else
                        current_word_bits_left = 32;
                }
                acca = (Int32)out_word_index << 4;
                acca = acca - (Int32)number_of_bits_per_frame;
            }
        }
    }

    /* Fill out with 1's. */

    while (acca < 0)
    {
        temp = 16 - out_word_bits_free;
        slice = 0xffff >> temp;

        out_word = add(out_word, slice);
        out_words[out_word_index++] = out_word;

        out_word = 0;

        out_word_bits_free = 16;

        acca = (Int32)out_word_index << 4;
        acca = acca - (Int32)number_of_bits_per_frame;
    }
}

/***************************************************************************
 Function:    adjust_abs_region_power_index

 Syntax:      adjust_abs_region_power_index(Int16 *absolute_region_power_index,
                                            Int16 *mlt_coefs)

              inputs:   *mlt_coefs
                        *absolute_region_power_index
            
              outputs:  *absolute_region_power_index

 Description: Adjusts the absolute power index


 WMOPS:     7kHz |    24kbit    |      32kbit
          -------|--------------|----------------
            AVG  |    0.03      |      0.03
          -------|--------------|----------------
            MAX  |    0.12      |      0.12
          -------|--------------|----------------

           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.03      |     0.03       |     0.03
          -------|--------------|----------------|----------------
            MAX  |    0.14      |     0.14       |     0.14
          -------|--------------|----------------|----------------

***************************************************************************/
void adjust_abs_region_power_index(Int16 *absolute_region_power_index,Int16 *mlt_coefs)
{
    Int16 n,i;
    Int16 region;
    Int16 *raw_mlt_ptr;

    Int16 temp;

	raw_mlt_ptr = mlt_coefs;
    for (region = 0; region < NUMBER_OF_REGIONS; region++)
    {
        n = sub(absolute_region_power_index[region], 39);
        n = n >> 1;

        if (n > 0)
        {
            for (i = 0; i < REGION_SIZE; i++)
            {
                *raw_mlt_ptr++ >>= n;
            }

            temp = n << 1;
            temp = sub(absolute_region_power_index[region], temp);
            absolute_region_power_index[region] = temp;
        }
		else
			raw_mlt_ptr += REGION_SIZE;
    }
}

/***************************************************************************
 Function:    compute_region_powers

 Syntax:      Int16 compute_region_powers(Int16  *mlt_coefs,
                                           Int16  mag_shift,
                                           Int16  *drp_num_bits,
                                           UInt16 *drp_code_bits,
                                           Int16  *absolute_region_power_index)
                                                                   
 Description: Computes the power for each of the regions


 WMOPS:     7kHz |    24kbit    |    32kbit
          -------|--------------|---------------
            AVG  |    0.09      |    0.09
          -------|--------------|---------------
            MAX  |    0.13      |    0.13
          -------|--------------|---------------

           14kHz |    24kbit    |    32kbit     |     48kbit
          -------|--------------|---------------|----------------
            AVG  |    0.20      |    0.20       |     0.20
          -------|--------------|---------------|----------------
            MAX  |    0.29      |    0.29       |     0.29
          -------|--------------|---------------|----------------

***************************************************************************/

Int16 compute_region_powers(Int16  *mlt_coefs,
                             Int16  mag_shift,
                             Int16  *drp_num_bits,
                             Uint16 *drp_code_bits,
                             Int16  *absolute_region_power_index)
{
    Int16 *input_ptr;
    Int32 long_accumulator;
    Int16 power_shift;
    Int16 region;
    Int16 j;
    Int16 number_of_bits;

    Int32 acca;
    Int16 temp;
    Int16 temp1;

    input_ptr = mlt_coefs;
    for (region = 0; region < NUMBER_OF_REGIONS; region++)
    {
        long_accumulator = 0L;

        for (j = 0; j < REGION_SIZE; j++)
        {
            temp = *input_ptr++;
            long_accumulator = L_mac(long_accumulator, temp, temp);
        }
//printf("%x   ", long_accumulator);
        power_shift = norm_l(long_accumulator);
        long_accumulator = L_shl(long_accumulator, power_shift) >> 16;
        power_shift = 14 - power_shift;
/*
        long_accumulator >>= 1;
        power_shift = 0;

        acca = long_accumulator & 0x7fff0000L;

        while (acca > 0)
        {
            long_accumulator >>= 1;
            acca = long_accumulator & 0x7fff0000L;
            power_shift++;
        }
        acca = long_accumulator - 0x00007fffL;

        temp = power_shift + 15;
        while ((acca <= 0) && (temp >= 0))
        {
            long_accumulator = long_accumulator << 1;
            acca = long_accumulator - 0x00007fffL;
            power_shift--;
            temp = power_shift + 15;
        }
        long_accumulator >>= 1;
*/
//printf("%x %d\n", long_accumulator, power_shift);

        /* 28963 corresponds to square root of 2 times REGION_SIZE(20). */
        if (long_accumulator >= 28963)
            power_shift++;

        acca = (Int32)power_shift;
        acca = L_msu(acca, 1, mag_shift);
//        acca = (Int32)mag_shift << 1;
//        acca = (Int32)power_shift - acca;
        acca = acca + 35 - REGION_POWER_TABLE_NUM_NEGATIVES;
        absolute_region_power_index[region] = (Int16)acca;
    }


    /* Before we differentially encode the quantized region powers, adjust upward the
    valleys to make sure all the peaks can be accurately represented. */
    for (region = NUMBER_OF_REGIONS-2; region >= 0; region--)
    {
        temp1 = sub(absolute_region_power_index[region+1], DRP_DIFF_MAX);

        if (absolute_region_power_index[region] < temp1)
            absolute_region_power_index[region] = temp1;
    }

    /* The MLT is currently scaled too low by the factor
       ENCODER_SCALE_FACTOR(=18318)/32768 * (1./sqrt(160).
       This is the ninth power of 1 over the square root of 2.
       So later we will add ESF_ADJUSTMENT_TO_RMS_INDEX (now 9)
       to drp_code_bits[0]. */

    /* drp_code_bits[0] can range from 1 to 31. 0 will be used only as an escape sequence. */
    if (absolute_region_power_index[0] < 1-ESF_ADJUSTMENT_TO_RMS_INDEX)
        absolute_region_power_index[0] = 1-ESF_ADJUSTMENT_TO_RMS_INDEX;

    if (absolute_region_power_index[0] > 31-ESF_ADJUSTMENT_TO_RMS_INDEX)
        absolute_region_power_index[0] = 31-ESF_ADJUSTMENT_TO_RMS_INDEX;

    number_of_bits = 5;

    drp_num_bits[0] = 5;

    drp_code_bits[0] = add(absolute_region_power_index[0],ESF_ADJUSTMENT_TO_RMS_INDEX);

    /* Lower limit the absolute region power indices to -8 and upper limit them to 31. Such extremes
     may be mathematically impossible anyway.*/
    for (region = 1; region < NUMBER_OF_REGIONS; region++)
    {
        if (absolute_region_power_index[region] < -8-ESF_ADJUSTMENT_TO_RMS_INDEX)
            absolute_region_power_index[region] = -8-ESF_ADJUSTMENT_TO_RMS_INDEX;
        if (absolute_region_power_index[region] > 31-ESF_ADJUSTMENT_TO_RMS_INDEX)
            absolute_region_power_index[region] = 31-ESF_ADJUSTMENT_TO_RMS_INDEX;

        j = sub(absolute_region_power_index[region], absolute_region_power_index[region-1]);

        if (j < DRP_DIFF_MIN)	j = DRP_DIFF_MIN;

        temp = add(absolute_region_power_index[region-1], j);
        absolute_region_power_index[region] = temp;

        j -= DRP_DIFF_MIN;
        number_of_bits = add(number_of_bits,differential_region_power_bits[region][j]);
        drp_num_bits[region] = differential_region_power_bits[region][j];
        drp_code_bits[region] = differential_region_power_codes[region][j];
    }

    return (number_of_bits);
}

/***************************************************************************
 Function:    vector_huffman

 Syntax:      Int16 vector_huffman(Int16  category,
                                    Int16  power_index,
                                    Int16  *raw_mlt_ptr,
                                    UInt32 *word_ptr)

              inputs:     Int16  category
                          Int16  power_index
                          Int16  *raw_mlt_ptr

              outputs:    number_of_region_bits
                          *word_ptr


 Description: Huffman encoding for each region based on category and power_index

 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    0.03      |     0.03
          -------|--------------|----------------
            MAX  |    0.04      |     0.04
          -------|--------------|----------------

           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.03      |     0.03       |     0.03
          -------|--------------|----------------|----------------
            MAX  |    0.04      |     0.04       |     0.04
          -------|--------------|----------------|----------------

***************************************************************************/
Int16 vector_huffman(Int16 category,
                      Int16 power_index,
                      Int16 *raw_mlt_ptr,
                      Uint32 *word_ptr)
{
    Int16  inv_of_step_size_times_std_dev;
    Int16  j,n;
    Int16  k;
    Int16  number_of_region_bits;
    Int16  number_of_non_zero;
    Int16  vec_dim;
    Int16  num_vecs;
    Int16  kmax, kmax_plus_one;
    Int16  index,signs_index;
    Int16  *bitcount_table_ptr;
    Uint16 *code_table_ptr;
    Int32  code_bits;
    Int16  number_of_code_bits;
    Uint32 current_word;
    Int16  current_word_bits_free;

    Int32 acca;

    Int16 mytemp;			 /* new variable in Release 1.2 */
    Int32 myacca;			 /* new variable in Release 1.2 */

    /* initialize variables */
    vec_dim = vector_dimension[category];

    num_vecs = number_of_vectors[category];

    kmax = max_bin[category];

    kmax_plus_one = kmax + 1;

    current_word = 0L;

    current_word_bits_free = 32;

    number_of_region_bits = 0;

    /* set up table pointers */
    bitcount_table_ptr = (Int16 *)table_of_bitcount_tables[category];
    code_table_ptr = (Uint16 *)table_of_code_tables[category];

    /* compute inverse of step size * standard deviation */
    acca = (Int32)step_size_inverse_table[category] * standard_deviation_inverse_table[power_index];
    acca = acca + 4096;
    acca = acca >> 13;

	/*
	 *  The next two lines are new to Release 1.2
	 */

    mytemp = (Int16)(acca & 0x3);
    acca = acca >> 2;

    inv_of_step_size_times_std_dev = (Int16) acca;

    for (n = 0; n < num_vecs; n++)
    {
        index = 0;

        signs_index = 0;

        number_of_non_zero = 0;

        for (j = 0; j < vec_dim; j++)
        {
            k = abs_s(*raw_mlt_ptr);

            acca = (Int32)k * inv_of_step_size_times_std_dev;

			/*
			 *  The next four lines are new to Release 1.2
			 */

			myacca = (Int32)k * mytemp;
			myacca = L_add(myacca, (Int32)int_dead_zone_low_bits[category]);
			myacca = myacca >> 2;

            acca = L_add(acca, (Int32)int_dead_zone[category]);

			/*
			 *  The next two lines are new to Release 1.2
			 */

			acca = L_add(acca, myacca);
			acca = acca >> 13;

            k = (Int16) acca;
            if (k != 0)
            {
                number_of_non_zero = add(number_of_non_zero, 1);
                signs_index = shl(signs_index, 1);

                if (*raw_mlt_ptr > 0)
                    signs_index = add(signs_index, 1);

                if (k > kmax)
                    k = kmax;
            }
            acca = (Int32)index * kmax_plus_one;
            index = (Int16) acca;
            index = add(index, k);
            raw_mlt_ptr++;
        }

        code_bits = *(code_table_ptr+index);
        number_of_code_bits = add((*(bitcount_table_ptr+index)), number_of_non_zero);
        number_of_region_bits = add(number_of_region_bits, number_of_code_bits);

        acca = code_bits << number_of_non_zero;
        code_bits = L_add(acca, (Int32)signs_index);

        /* msb of codebits is transmitted first. */
        j = sub(current_word_bits_free, number_of_code_bits);

        if (j < 0)
        {
            current_word = current_word + (code_bits >> (-j));
            *word_ptr++ = current_word;
            current_word_bits_free = 32 + j;
            current_word = code_bits << current_word_bits_free;
        }
        else
        {
            current_word_bits_free = j;
            current_word = current_word + (code_bits << j);
        }
    }

    *word_ptr++ = current_word;

    return (number_of_region_bits);
}

/***************************************************************************
 Function:    vector_quantize_mlts

 Syntax:      void vector_quantize_mlts(number_of_available_bits,
                                        mlt_coefs,
                                        absolute_region_power_index,
                                        power_categories,
                                        category_balances,
                                        p_categorization_control,
                                        region_mlt_bit_counts,
                                        region_mlt_bits)


 Description: Scalar quantized vector Huffman coding (SQVH)


 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    0.57      |     0.65
          -------|--------------|----------------
            MAX  |    0.78      |     0.83
          -------|--------------|----------------

           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.62      |     0.90       |     1.11
          -------|--------------|----------------|----------------
            MAX  |    1.16      |     1.39       |     1.54
          -------|--------------|----------------|----------------

***************************************************************************/

void vector_quantize_mlts(Int16 number_of_available_bits,
                          Int16 *mlt_coefs,
                          Int16 *absolute_region_power_index,
                          Int16 *power_categories,
                          Int16 *category_balances,
                          Int16 *p_categorization_control,
                          Int16 *region_mlt_bit_counts,
                          Uint32 *region_mlt_bits)
{
    Int16 *raw_mlt_ptr;
    Int16 region;
    Int16 category;
    Int16 total_mlt_bits = 0;

    Int16 temp;

    /* Start in the middle of the categorization control range. */
    temp = NUM_CATEGORIZATION_CONTROL_POSSIBILITIES >> 1;
    temp = temp - 1;
    for (*p_categorization_control = 0; *p_categorization_control < temp; (*p_categorization_control)++)
    {
        region = category_balances[*p_categorization_control];
        power_categories[region] = add(power_categories[region], 1);
    }

	raw_mlt_ptr = mlt_coefs;
    for (region = 0; region < NUMBER_OF_REGIONS; region++)
    {
        category = power_categories[region];
        if (category < NUM_CATEGORIES - 1)
        {
			temp = region << 2;
            region_mlt_bit_counts[region] =
				vector_huffman(category, 
								absolute_region_power_index[region],
								raw_mlt_ptr,
								&region_mlt_bits[temp]);
            total_mlt_bits = add(total_mlt_bits, region_mlt_bit_counts[region]);
        }
        else
        {
            region_mlt_bit_counts[region] = 0;
        }
		raw_mlt_ptr += REGION_SIZE;
    }

    /* If too few bits... */
    while ((total_mlt_bits < number_of_available_bits)
    	&& (*p_categorization_control > 0))
    {
        (*p_categorization_control)--;
        region = category_balances[*p_categorization_control];

        power_categories[region] = sub(power_categories[region], 1);

        total_mlt_bits = sub(total_mlt_bits, region_mlt_bit_counts[region]);
        category = power_categories[region];

        raw_mlt_ptr = &mlt_coefs[region*REGION_SIZE];

        if (category < NUM_CATEGORIES - 1)
        {
			temp = region << 2;
            region_mlt_bit_counts[region] =
                vector_huffman(category,
                               absolute_region_power_index[region],
                               raw_mlt_ptr,
                               &region_mlt_bits[temp]);
            total_mlt_bits = add(total_mlt_bits, region_mlt_bit_counts[region]);
        }
        else
        {
            region_mlt_bit_counts[region] = 0;
        }
    }

    /* If too many bits... */
    /* Set up for while loop test */
    while ((total_mlt_bits > number_of_available_bits) &&
           (*p_categorization_control < NUM_CATEGORIZATION_CONTROL_POSSIBILITIES - 1))
    {
        /* operations for while contitions */

        region = category_balances[*p_categorization_control];

        power_categories[region] = add(power_categories[region], 1);

        total_mlt_bits = sub(total_mlt_bits, region_mlt_bit_counts[region]);
        category = power_categories[region];

        raw_mlt_ptr = &mlt_coefs[region*REGION_SIZE];

        if (category < NUM_CATEGORIES-1)
        {
			temp = region << 2;
            region_mlt_bit_counts[region] =
                vector_huffman(category,
                               absolute_region_power_index[region],
                               raw_mlt_ptr,
                               &region_mlt_bits[temp]);
            total_mlt_bits = add(total_mlt_bits, region_mlt_bit_counts[region]);
        }
        else
        {
            region_mlt_bit_counts[region] = 0;
        }
        (*p_categorization_control)++;
    }
}

/***************************************************************************
 Function:    encoder

 Syntax:      void encoder(Int16 number_of_available_bits,
                           Int16 mlt_coefs,
                           Int16 mag_shift,
                           Int16 out_words)

              inputs:   number_of_available_bits
                        mag_shift
                        mlt_coefs[DCT_LENGTH]

              outputs:  out_words[MAX_BITS_PER_FRAME/16]


 Description: Encodes the mlt coefs into out_words using G.722.1 Annex C


 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    0.93      |     1.04
          -------|--------------|----------------
            MAX  |    1.20      |     1.28
          -------|--------------|----------------

           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    1.39      |     1.71       |     2.01
          -------|--------------|----------------|----------------
            MAX  |    2.00      |     2.30       |     2.52
          -------|--------------|----------------|----------------

***************************************************************************/

void encoder(Int16  number_of_available_bits,
             Int16  *mlt_coefs,
             Int16  mag_shift,
             Int16  *out_words)
{
    Int16  number_of_bits_per_frame;
    Int16  number_of_envelope_bits;
    Int16  categorization_control;
    Int16  region;
    Int16  absolute_region_power_index[NUMBER_OF_REGIONS];
    Int16  power_categories[NUMBER_OF_REGIONS];
    Int16  category_balances[NUM_CATEGORIZATION_CONTROL_POSSIBILITIES-1];
    Int16  drp_num_bits[NUMBER_OF_REGIONS+1];
    Uint16 drp_code_bits[NUMBER_OF_REGIONS+1];
    Int16  region_mlt_bit_counts[NUMBER_OF_REGIONS];
    Uint32 region_mlt_bits[4*NUMBER_OF_REGIONS];
    Int16  mag_shift_offset;

    Int16 temp;

    number_of_bits_per_frame = number_of_available_bits;

    for (region = 0; region < NUMBER_OF_REGIONS; region++)
    {
        region_mlt_bit_counts[region] = 0;
    }

    /* Estimate power envelope. */
    number_of_envelope_bits = compute_region_powers(mlt_coefs,
                                                    mag_shift,
                                                    drp_num_bits,
                                                    drp_code_bits,
                                                    absolute_region_power_index);

    /* Adjust number of available bits based on power envelope estimate */
    temp = sub(number_of_available_bits,number_of_envelope_bits);
    number_of_available_bits = sub(temp, NUM_CATEGORIZATION_CONTROL_BITS);

    /* get categorizations */
    categorize(number_of_available_bits,
               absolute_region_power_index,
               power_categories,
               category_balances);

    /* Adjust absolute_region_category_index[] for mag_shift.
       This assumes that REGION_POWER_STEPSIZE_DB is defined
       to be exactly 3.010299957 or 20.0 times log base 10
       of square root of 2. */
    temp = shl(mag_shift, 1);
    mag_shift_offset = add(temp, REGION_POWER_TABLE_NUM_NEGATIVES);

    for (region = 0; region < NUMBER_OF_REGIONS; region++)
    {
        absolute_region_power_index[region] = add(absolute_region_power_index[region], mag_shift_offset);
    }

    /* adjust the absolute power region index based on the mlt coefs */
    adjust_abs_region_power_index(absolute_region_power_index, mlt_coefs);

    /* quantize and code the mlt coefficients based on categorizations */
    vector_quantize_mlts(number_of_available_bits,
                         mlt_coefs,
                         absolute_region_power_index,
                         power_categories,
                         category_balances,
                         &categorization_control,
                         region_mlt_bit_counts,
                         region_mlt_bits);

    /* stuff bits into words */
    bits_to_words(region_mlt_bits,
                  region_mlt_bit_counts,
                  drp_num_bits,
                  drp_code_bits,
                  out_words,
                  categorization_control,
                  number_of_bits_per_frame);
}

