/***********************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   2004 Polycom, Inc.
**
**   All rights reserved.
**
***********************************************************************/

#include "codec.h"
#include "typedef.h"

#ifdef	BW14K
	#define DCT_LENGTH			640
	#define NUMBER_OF_REGIONS	28
	#define NUM_CATEGORIZATION_CONTROL_BITS				5
	#define DCT_LENGTH_LOG		7

#else
	#define DCT_LENGTH			320
	#define NUMBER_OF_REGIONS	14
	#define NUM_CATEGORIZATION_CONTROL_BITS				4
	#define DCT_LENGTH_LOG		6

#endif	// BANDWIDTH 7k or 14k

/*  region_size = (BLOCK_SIZE * 0.875)/NUM_REGIONS; */
#define REGION_SIZE             20
#define NUMBER_OF_VALID_COEFS   (NUMBER_OF_REGIONS * REGION_SIZE)
#define NUM_CATEGORIZATION_CONTROL_POSSIBILITIES \
	(1<<NUM_CATEGORIZATION_CONTROL_BITS)

#define	MAX_NUMBER_OF_REGIONS	28

#define DCT_LENGTH_DIV_2    160
#define DCT_LENGTH_DIV_4     80
#define DCT_LENGTH_DIV_8     40
#define DCT_LENGTH_DIV_16    20
#define DCT_LENGTH_DIV_32    10
#define DCT_LENGTH_DIV_64     5


#define NUM_CATEGORIES        8
#define CORE_SIZE            10


#define REGION_POWER_TABLE_SIZE 64
#define REGION_POWER_TABLE_NUM_NEGATIVES 24


#define ENCODER_SCALE_FACTOR 18318.0

/* The MLT output is incorrectly scaled by the factor
   product of ENCODER_SCALE_FACTOR and sqrt(160.)
   This is now (9/30/96) 1.0/2^(4.5) or 1/22.627.
   In the current implementation this  
   must be an integer power of sqrt(2). The
   integer power is ESF_ADJUSTMENT_TO_RMS_INDEX.
   The -2 is to conform with the range defined in the spec. */

 
#define ESF_ADJUSTMENT_TO_RMS_INDEX (9-2)
 
/* Max bit rate is 48000 bits/sec. */
#define MAX_BITS_PER_FRAME 960


/***************************************************************************/
/* Function definitions                                                    */
/***************************************************************************/

void encoder(Int16  number_of_available_bits,
                Int16  *mlt_coefs,
                Int16  mag_shift,
                Int16  *out_words);

void decoder(Int16 *out_words,
             Int16 *decoder_mlt_coefs,
	         Int16 *p_mag_shift,
	         Int16 *p_old_mag_shift);

void dct_type_iv_a (Int16 *input, Int16 *output, Int16 dct_length);
Int16 samples_to_rmlt_coefs(Int16 *new_samples, Int16 *coefs,
					Uint16 chn);

void dct_type_iv_s (Int16 *input, Int16 *output, Int16 dct_length);
void rmlt_coefs_to_samples(Int16 *coefs, Int16 *out_samples,
					Int16 mag_shift, Uint16 chn);

void categorize(Int16 number_of_available_bits,
		           Int16 *rms_index,
		           Int16 *power_categories,
		           Int16 *category_balances);

void DSP_zero(Int16 *buf, Int16 cnt);
void DSP_memcpy(Int16 *dst, const Int16 *src, Int16 cnt);
void DSP_add(Int16 *dst, const Int16 *src, Int16 cnt);
Int16 DSP_mac(Int16 *ptr1, Int16 *ptr2, Int16 cnt);
void DSP_byte2word(Uint16 *dst, const Uint8 *src, Int16 cnt);
void DSP_word2byte(Uint8 *dst, const Uint16 *src, Int16 cnt);

/*****************************************************
 * prototype in main
 ****************************************************/
void aic3204_init(int rate);
void EZDSP5535_I2S_init(void);
void TIMER_init(void);
void INTR_init(void);
void PLL_init(void);
void DMA_audio_init(Uint16 size);

void sam2coef_init(void);
void coef2sam_init(void);

