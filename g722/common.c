/****************************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   2004 Polycom, Inc.
**
**	 All rights reserved.
**
****************************************************************************************/

/****************************************************************************************
  Filename:    common.c    

  Purpose:     Contains the functions used for both G.722.1 Annex C encoder and decoder
		
  Design Notes:

****************************************************************************************/
/****************************************************************************************
 Include files                                                           
****************************************************************************************/
#include "codec.h"
#include "defs.h"
#include "huff_def.h"
#include "tables.h"

#include "basop32.h"

/***************************************************************************
 Function:    comp_powercat_and_catbalance

 Syntax:      void comp_powercat_and_catbalance(Int16 *power_categories,
                                                Int16 *category_balances,
                                                Int16 *rms_index,  
                                                Int16 number_of_available_bits,                                  
                                                Int16 num_categorization_control_possibilities,
                                                Int16 offset)                                                    

                
                inputs:   *rms_index                              
                          number_of_available_bits                 
                          num_categorization_control_possibilities
                          offset
                           
                outputs:  *power_categories  
                          *category_balances 
                
                
 Description: Computes the power_categories and the category balances

 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    0.10      |     0.10
          -------|--------------|----------------  
            MAX  |    0.11      |     0.11
          -------|--------------|---------------- 
				
           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.32      |     0.35       |     0.38   
          -------|--------------|----------------|----------------
            MAX  |    0.38      |     0.42       |     0.43   
          -------|--------------|----------------|----------------

***************************************************************************/
void comp_powercat_and_catbalance(Int16 *power_categories,
                                  Int16 *category_balances,
                                  Int16 *rms_index,
                                  Int16 number_of_available_bits,
								  Int16 offset)
{
    Int16 expected_number_of_code_bits;
    Int16 region;
    Int16 j;
    Int16 max_rate_categories[NUMBER_OF_REGIONS];
    Int16 min_rate_categories[NUMBER_OF_REGIONS];
    Int16 temp_category_balances[2*NUM_CATEGORIZATION_CONTROL_POSSIBILITIES];
    Int16 raw_max, raw_min;
    Int16 raw_max_index, raw_min_index;
    Int16 max_rate_pointer, min_rate_pointer;
    Int16 max, min;
    Int16 itemp0;
    Int16 itemp1;
    Int16 min_plus_max;

    expected_number_of_code_bits = 0;

    for (region = 0; region < NUMBER_OF_REGIONS; region++) 
    {
        j = sub(offset, rms_index[region]);
        
        /* make sure j is between 0 and NUM_CAT-1 */
        if (j < 0) j = 0;
        else j = j >> 1;
        if(j >= NUM_CATEGORIES)
            j = NUM_CATEGORIES - 1;
        
    /* compute the power categories based on the uniform offset */
        power_categories[region] = j;

        expected_number_of_code_bits = add(expected_number_of_code_bits, expected_bits_table[j]);
        max_rate_categories[region] = j;
        min_rate_categories[region] = j;
    }
    
    max = expected_number_of_code_bits;
    min = expected_number_of_code_bits;
    max_rate_pointer = NUM_CATEGORIZATION_CONTROL_POSSIBILITIES;
    min_rate_pointer = NUM_CATEGORIZATION_CONTROL_POSSIBILITIES;
    
    for (j = 0; j < NUM_CATEGORIZATION_CONTROL_POSSIBILITIES - 1; j++) 
    {
        min_plus_max = add(max, min);
        itemp0 = shl(number_of_available_bits, 1);
        
        if (min_plus_max <= itemp0)
        {
            raw_min = 99;
            /* Search from lowest freq regions to highest for best */
            /* region to reassign to a higher bit rate category.   */
            for (region=0; region < NUMBER_OF_REGIONS; region++) 
            {
                if (max_rate_categories[region] > 0) 
                {
                    itemp0 = shl(max_rate_categories[region], 1);
                    itemp1 = sub(offset, rms_index[region]);
                    itemp0 = sub(itemp1, itemp0);
                    
                    if (itemp0 < raw_min)
                    {
                        raw_min = itemp0;
                        raw_min_index = region;
                    }
                }
            }
            max_rate_pointer--;
            temp_category_balances[max_rate_pointer] = raw_min_index;

        	max = sub(max, expected_bits_table[max_rate_categories[raw_min_index]]);
        	max_rate_categories[raw_min_index]--;
            max = add(max, expected_bits_table[max_rate_categories[raw_min_index]]);
        }
        else 
        {
            raw_max = -99;
            /* Search from highest freq regions to lowest for best region to reassign to
            a lower bit rate category. */
            for (region= NUMBER_OF_REGIONS-1; region >= 0; region--) 
            {
                if (min_rate_categories[region] < (NUM_CATEGORIES-1))
                {
                    itemp0 = shl(min_rate_categories[region], 1);
                    itemp1 = sub(offset, rms_index[region]);
                    itemp0 = sub(itemp1, itemp0);
                    
                    if (itemp0 > raw_max)
                    {
                        raw_max = itemp0;
                        raw_max_index = region;
                    }
                }
            }
            temp_category_balances[min_rate_pointer] = raw_max_index;
            
            min_rate_pointer++;
            min = sub(min, expected_bits_table[min_rate_categories[raw_max_index]]);
            min_rate_categories[raw_max_index]++;
            min = add(min, expected_bits_table[min_rate_categories[raw_max_index]]);
        }
    }
    
    for (region = 0; region < NUMBER_OF_REGIONS; region++)
        power_categories[region] = max_rate_categories[region];
    
    for (j = 0; j < NUM_CATEGORIZATION_CONTROL_POSSIBILITIES - 1; j++)
        category_balances[j] = temp_category_balances[max_rate_pointer++];

}

/***************************************************************************
 Function:    calc_offset

 Syntax:      offset=calc_offset(Int16 *rms_index,Int16 number_of_regions,Int16 available_bits)  

                input:  Int16 *rms_index
                        Int16 number_of_regions
                        Int16 available_bits
                        
                output: Int16 offset                        

 Description: Calculates the the category offset.  This is the shift required
              To get the most out of the number of available bits.  A binary 
              type search is used to find the offset.

 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    0.04      |     0.04
          -------|--------------|----------------  
            MAX  |    0.04      |     0.04
          -------|--------------|---------------- 

           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.08      |     0.08       |     0.08   
          -------|--------------|----------------|----------------
            MAX  |    0.09      |     0.09       |     0.09   
          -------|--------------|----------------|----------------

***************************************************************************/
Int16 calc_offset(Int16 *rms_index, Int16 available_bits)
{
    Int16 answer;
    Int16 delta;
    Int16 test_offset;
    Int16 region,j;
    Int16 bits;
    Int16 offset;

    /* initialize vars */
    answer = -32;
    delta = 32;
    
    do 
    {
        test_offset = add(answer, delta);

        /* compute the number of bits that will be used given the cat assignments */
        bits = 0;
        /* obtain a category for each region */
        /* using the test offset             */
        for (region = 0; region< NUMBER_OF_REGIONS; region++) 
        {
            j = sub(test_offset, rms_index[region]);
            
            /* Ensure j is between 0 and NUM_CAT-1 */
            if (j < 0) j = 0;
            else j = j >> 1;
            if(j >= NUM_CATEGORIES)
                j = NUM_CATEGORIES - 1;

            bits = add(bits, expected_bits_table[j]);
        }

        /* if (bits > available_bits - 32) then divide the offset region for the bin search */
        offset = sub(available_bits, 32);
        if (bits >= offset)
            answer = test_offset;
        delta = delta >> 1;
    } while (delta > 0);

    return(answer);
}

/****************************************************************************************
 Function:    categorize

 Syntax:      void categorize(Int16 number_of_available_bits,   
                              Int16 num_categorization_control_possibilities,
                              Int16 rms_index,                  
                              Int16 power_categories,           
                              Int16 category_balances)          

                  inputs:   num_categorization_control_possibilities
                            number_of_available_bits
                            rms_index[MAX_NUMBER_OF_REGIONS]                              
                  
                  outputs:  power_categories[MAX_NUMBER_OF_REGIONS]                       
                            category_balances[MAX_NUM_CATEGORIZATION_CONTROL_POSSIBILITIES-1]

 Description: Computes a series of categorizations 

 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    0.14      |     0.14
          -------|--------------|----------------  
            MAX  |    0.15      |     0.15
          -------|--------------|---------------- 
   			
           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    0.42      |     0.45       |     0.48   
          -------|--------------|----------------|----------------
            MAX  |    0.47      |     0.52       |     0.52   
          -------|--------------|----------------|----------------

****************************************************************************************/
void categorize(Int16 number_of_available_bits,
		        Int16 *rms_index,
		        Int16 *power_categories,
		        Int16 *category_balances)
{
    Int16 offset;
    Int16 temp, temp1;

    /* At higher bit rates, there is an increase for most categories in average bit
       consumption per region. We compensate for this by pretending we have fewer
       available bits. */

    temp = number_of_available_bits - DCT_LENGTH;

    if (temp > 0)
    {
		temp1 = temp >> 2;
		temp = temp + temp1;
		number_of_available_bits = temp >> 1;
		number_of_available_bits += DCT_LENGTH;
    }

    /* calculate the offset using the original category assignments */
    offset = calc_offset(rms_index, number_of_available_bits);

    /* adjust the category assignments */
    /* compute the new power categories and category balances */
    comp_powercat_and_catbalance(power_categories,category_balances,rms_index,number_of_available_bits, offset);
}

