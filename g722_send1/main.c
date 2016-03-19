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
#include "ezdsp5535_spi.h"
#include "csl_spi.h"
#include "defs.h"
#include "csl_mmcsd.h"
#include "fat.h"
#include "Si446x_api.h"

#define	ICR		(*(volatile ioport Uint16 *)0x0001)

#pragma DATA_SECTION(rcv_l, "dmaMem")
#pragma DATA_SECTION(xmt_l, "dmaMem")
#pragma DATA_SECTION(rcv_r, "dmaMem")
#pragma DATA_SECTION(xmt_r, "dmaMem")
#pragma DATA_ALIGN (rcv_l, 4)
#pragma DATA_ALIGN (xmt_l, 4)
#pragma DATA_ALIGN (rcv_r, 4)
#pragma DATA_ALIGN (xmt_r, 4)
Int16 rcv_l[DCT_LENGTH*2], xmt_l[DCT_LENGTH*2];
Int16 rcv_r[DCT_LENGTH*2], xmt_r[DCT_LENGTH*2];

Int16 buffer_a[DCT_LENGTH], buffer_b[DCT_LENGTH], buffer_c[DCT_LENGTH];
Int16 windowed_data[DCT_LENGTH];
volatile Int16 pingpong = -1;
volatile Int16 gpioIsrStatus = 0;
volatile Int16 spiIsrStatus = 0;

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

Int16  rawOutWords[MAX_BITS_PER_FRAME/16+1];
Uint8  RfByte[2*(MAX_BITS_PER_FRAME/8+2)];		// left and right
void main(void)
{
	Int16  *samples;
	HANDLE FHandle = Not_Open_FILE;
	Int16  *out_words = &rawOutWords[1];
	Uint8  *byte16 = &RfByte[2];	
	Int16  mlt_coefs[DCT_LENGTH];
	Int16  mag_shift[2];
	Int16  old_mag_shift[2];
	Uint16 number_of_bits_per_frame;
	Uint16 number_of_bytes_per_frame;
	Uint16 number_of_16bit_words_per_frame;
	Int16  offset;
	Int16  key;
	Uint16 chn;		// left and right channel, 0 or 1
	Int16  sent_buffer_cnt = 0;
	Uint8  *sent_buffer;
	
	ICR = 0xff2e;	/* IDLE Control Register */
					/* mem port, io port and cpu idle active */

	DSP_zero(xmt_l, DCT_LENGTH);
	DSP_zero(xmt_r, DCT_LENGTH);
	/* Peripheral_Reset  */
	CSL_SYSCTRL_REGS->PSRCR = 0x0020;

	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;
	CSL_SYSCTRL_REGS->PRCR = 0x00bf;

	CSL_SYSCTRL_REGS->EBSR = (CSL_SYS_EBSR_PPMODE_MODE1 << 12)
							| (0 << 8) | (2 << 10);//GPIO(11-6)
	asm("	bit(ST1, #ST1_INTM) = #1");

	number_of_bits_per_frame = (Uint16)(BITRATE * 20);	// BITRATE in kbps
	number_of_bytes_per_frame = (Uint16)(number_of_bits_per_frame>>3);
	number_of_16bit_words_per_frame = (Uint16)(number_of_bits_per_frame>>4);

	INTR_init();
	PLL_init();
	TIMER_init();
	EZDSP5535_GPIO_init();
	DMA_audio_init(DCT_LENGTH);

	EZDSP5535_I2C_init();		/* Initialize I2C */
	aic3204_init(6);			/* Initialize codec */
	EZDSP5535_LCD_init();
	EZDSP5535_I2S_init();		/* Initialize I2S */
	LCD_print("G722.1 Send       ", 17, 0);
	LCD_print("16kbps  ", 8, 1);
	EZDSP5535_SPI_init();
	Si446x_Init();
	EZDSP5535_SAR_init();
	DiskInit();
	FileInit();
	AddFileDriver(SDCammand, NULL);

	sam2coef_init();
	coef2sam_init();
	rawOutWords[0] = 0x6b21;
	RfByte[0] = 0x21; RfByte[1] = 0x6b;		// little-endian

	old_mag_shift[0] = 0;
	old_mag_shift[1] = 0;
	asm("	bit(ST1, #ST1_INTM) = #0");

	SpiWriteByte(CMD_GET_INT_STATUS, 0);	// clear interrupt pending
	while(1) {
		if(pingpong >= 0) {
			offset = (pingpong>>1) * DCT_LENGTH;
/*******************************************************************
 * LEFT LEFT LEFT LEFT LEFT LEFT LEFT LEFT LEFT LEFT LEFT LEFT
 *******************************************************************/
			chn = LEFT;
			samples = &rcv_l[offset];		// Read frame of samples from mem.
			// Convert input samples to rmlt coefs
			mag_shift[chn] = samples_to_rmlt_coefs(samples, mlt_coefs, chn);
			// Encode the mlt coefs
			encoder(number_of_bits_per_frame, mlt_coefs, mag_shift[chn], out_words);
			byte16 = &RfByte[2];
			byte16[-2] = 0x6b; byte16[-1] = 0x20;
			DSP_word2byte(byte16, out_words, number_of_16bit_words_per_frame);
			
			// process the out_words into decoder_mlt_coefs
			decoder(out_words, mlt_coefs, &mag_shift[chn], &old_mag_shift[chn]);

			samples = &xmt_l[offset];		// Write frame of output samples
			// convert the decoder_mlt_coefs to samples
			rmlt_coefs_to_samples(mlt_coefs, samples, mag_shift[chn], chn);

/*******************************************************************
 * RIGHT RIGHT RIGHT RIGHT RIGHT RIGHT RIGHT RIGHT RIGHT RIGHT 
 *******************************************************************/
			chn = RIGHT;
			samples = &rcv_r[offset];		// Read frame of samples from mem.
			// Convert input samples to rmlt coefs
			mag_shift[chn] = samples_to_rmlt_coefs(samples, mlt_coefs, chn);
			// Encode the mlt coefs
			encoder(number_of_bits_per_frame, mlt_coefs, mag_shift[chn], out_words);

			byte16 = &RfByte[number_of_bytes_per_frame + 4];
			byte16[-2] = 0x6b; byte16[-1] = 0x21;
			DSP_word2byte(byte16, out_words, number_of_16bit_words_per_frame);

			// process the out_words into decoder_mlt_coefs
			decoder(out_words, mlt_coefs, &mag_shift[chn], &old_mag_shift[chn]);

			samples = &xmt_r[offset];		// Write frame of output samples
			// convert the decoder_mlt_coefs to samples
			rmlt_coefs_to_samples(mlt_coefs, samples, mag_shift[chn], chn);

			if (FHandle != Not_Open_FILE)	// Write output bitstream
				FileWrite(RfByte, 2*number_of_bytes_per_frame+4, FHandle);

			pingpong = -1;
			sent_buffer = RfByte;
			sent_buffer_cnt = 0;
		}
		if(spiIsrStatus == 1) {
			bApi_Set_Send(43);
			spiIsrStatus = 0;
		}
		if(sent_buffer_cnt < 2) {
			if(gpioIsrStatus == 1) {
				SPI_sendData(sent_buffer, number_of_bytes_per_frame+2);
				sent_buffer += number_of_bytes_per_frame+2;
				sent_buffer_cnt++;
				gpioIsrStatus = 0;
				CSL_GPIO_REGS->IOINTFLG1 = 0x0800;
			}
		}

//		key = EZDSP5535_SAR_getKey();
		key = -1;
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

