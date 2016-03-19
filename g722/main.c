//////////////////////////////////////////////////////////////////////////////
// * File name: main.c
// *                                                                          
// * Description:  Main function.
// *                                                                          
/*************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**

 Description:  Contains the main function for the G.722.1 Annex C encoder

 Design Notes:
 
 WMOPS:     7kHz |    24kbit    |     32kbit
          -------|--------------|----------------
            AVG  |    2.32      |     2.43
          -------|--------------|----------------  
            MAX  |    2.59      |     2.67
          -------|--------------|---------------- 
                                                                        
           14kHz |    24kbit    |     32kbit     |     48kbit
          -------|--------------|----------------|----------------
            AVG  |    4.45      |     4.78       |     5.07   
          -------|--------------|----------------|----------------
            MAX  |    5.07      |     5.37       |     5.59   
          -------|--------------|----------------|----------------

*************************************************************************/

/***************************************************************************
 Include files                                                           
****************************************************************************/

#include "ezdsp5535_gpio.h"
#include "ezdsp5535_i2c.h"
#include "ezdsp5535_lcd.h"
#include "ezdsp5535_sar.h"
#include "defs.h"
#include "csl_mmcsd.h"
#include "fat.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)


#pragma DATA_SECTION(rcv_l, ".cio")
#pragma DATA_SECTION(xmt_l, ".cio")
#pragma DATA_SECTION(rcv_r, ".cio")
#pragma DATA_SECTION(xmt_r, ".cio")
#pragma DATA_ALIGN (rcv_l, 4)
#pragma DATA_ALIGN (xmt_l, 4)
#pragma DATA_ALIGN (rcv_r, 4)
#pragma DATA_ALIGN (xmt_r, 4)
Int16 rcv_l[DCT_LENGTH*2], xmt_l[DCT_LENGTH*2];
Int16 rcv_r[DCT_LENGTH*2], xmt_r[DCT_LENGTH*2];

Int16 buffer_a[DCT_LENGTH], buffer_b[DCT_LENGTH], buffer_c[DCT_LENGTH];
Int16 windowed_data[DCT_LENGTH];
//volatile Int16 timer = 0;              // timer interrupt
volatile Uint16 t_cnt = 0;             // timer interrupt counter
volatile Int16 pingpong = -1;
volatile Uint16 s_cnt = 0;             // DMA interrupt counter

void sam2coef_init();
void coef2sam_init();
void word2byte(Uint8 *dst, Int16 *src, int cnt)
{
	while(cnt-- > 0)
	{
		*dst++ = (*src) & 0xff;
		*dst++ = (*src++) >> 8;
	}
}

/*
 *
 *  main( )
 *
 */

/***************************************************************************
 Function:     G.722.1 Annex C main encoder function 

            bit rate    16, 24, 32, and 48 kbps
            bandwidth   7 or 14 kHz (defined in codec.h)
                        
 Description:  Main processing loop for the G.722.1 Annex C encoder/decoder

 Design Notes: 16kbit/sec is only supported for bandwidth of 7kHz

****************************************************************************/
void main(void)
{
	Int16  *samples;
	HANDLE FHandle = Not_Open_FILE;
	Int16  out_words[MAX_BITS_PER_FRAME/16];
	Uint8  byte16[MAX_BITS_PER_FRAME/8];
	Int16  mlt_coefs[DCT_LENGTH];
	Int16  mag_shift;
	Int16  old_mag_shift=0;
	Uint16 number_of_bits_per_frame;
	Uint16 number_of_16bit_words_per_frame;
	Int16 offset;
	Int16 key;

	ICR = 0xff2e;	/* IDLE Control Register */
					/* mem port, io port and cpu idle active */

	DSP_zero(xmt_l, 2*DCT_LENGTH);
	DSP_zero(xmt_r, 2*DCT_LENGTH);
	/* Peripheral_Reset  */
	CSL_SYSCTRL_REGS->PSRCR = 0x0020;

	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;
	CSL_SYSCTRL_REGS->PRCR = 0x00bf;

	CSL_SYSCTRL_REGS->EBSR = (CSL_SYS_EBSR_PPMODE_MODE1 << 12)
							|(CSL_SYS_EBSR_SP1MODE_MODE2 << 10)
							|(CSL_SYS_EBSR_SP0MODE_MODE0 << 8);
	asm("	bit(ST1, #ST1_INTM) = #1");

	INTR_init();
	PLL_init(100000000);
	TIMER_init();
	EZDSP5535_GPIO_init();
	DMA_audio_init(DCT_LENGTH);

	EZDSP5535_I2C_init();		/* Initialize I2C */
	aic3204_init(6);			/* Initialize codec */
	EZDSP5535_LCD_init();
	EZDSP5535_I2S_init();		/* Initialize I2S */
	LCD_print("G722.1 TEST       ", 17, 0);
	LCD_print("16kbps  ", 8, 1);
	LCD_scroll();
	EZDSP5535_SAR_init();

	DiskInit();
	FileInit();

	AddFileDriver(SDCammand, NULL);

	sam2coef_init();
	coef2sam_init();

	number_of_bits_per_frame = (Uint16)(BITRATE * 20);	// BITRATE in kbps
	number_of_16bit_words_per_frame = (Uint16)(number_of_bits_per_frame/16);

	asm("	bit(ST1, #ST1_INTM) = #0");

	while(1) {
		if(pingpong >= 0) {
			offset = (pingpong>>1) * DCT_LENGTH;
			DSP_memcpy(&xmt_r[offset], &rcv_r[offset], DCT_LENGTH);

			/* Read frame of samples from mem. */
			samples = &rcv_l[offset];

			// Convert input samples to rmlt coefs
			mag_shift = samples_to_rmlt_coefs(samples, mlt_coefs);

			// Encode the mlt coefs
			encoder(number_of_bits_per_frame,
				mlt_coefs,
				mag_shift,
				out_words);

			word2byte(byte16, out_words, number_of_16bit_words_per_frame);
			// Write output bitstream
			if (FHandle != Not_Open_FILE)
				FileWrite(byte16, (number_of_16bit_words_per_frame << 1), FHandle);

			// process the out_words into decoder_mlt_coefs
			decoder(out_words,
				mlt_coefs,
				&mag_shift,
				&old_mag_shift);

			// Write frame of output samples
			samples = &xmt_l[offset];
			// convert the decoder_mlt_coefs to samples
			rmlt_coefs_to_samples(mlt_coefs, samples, mag_shift);

			pingpong = -1;
		}
		key = EZDSP5535_SAR_getKey();
		if(key == SW1)
		if(FHandle == Not_Open_FILE)
		{
			LCD_print("RECORDING......", 15, 0);
			FHandle = FileOpen("A:\\VOICE000.TXT", FILE_FLAGS_WRITE);
		}
		if(key == SW2)
		if(FHandle != Not_Open_FILE)
		{
			LCD_print("SAVE VOICE000.DAT", 17, 0); 
			FileClose(FHandle);
			AllCacheWriteBack();
			FHandle = Not_Open_FILE;
		}
		asm("	idle");
	}

//	RemoveFileDriver(SDCammand);
}
