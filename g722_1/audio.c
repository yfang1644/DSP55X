#include "ezdsp5535.h"
#include "g722_1.h"
#include "libg722_1.h"

#include "csl_intc.h"

// DMA1 channel 0 for I2S receive left
// DMA1 channel 1 for I2S transmit left
// DMA1 channel 2 for I2S receive right
// DMA1 channel 3 for I2S transmit right
#pragma DATA_SECTION(rcv_l, ".cio")
#pragma DATA_SECTION(xmt_l, ".cio")
#pragma DATA_SECTION(rcv_r, ".cio")
#pragma DATA_SECTION(xmt_r, ".cio")
#pragma DATA_ALIGN (rcv_l, 4)
#pragma DATA_ALIGN (xmt_l, 4)
#pragma DATA_ALIGN (rcv_r, 4)
#pragma DATA_ALIGN (xmt_r, 4)
Int16 rcv_l[MAX_DCT_LENGTH*2], xmt_l[MAX_DCT_LENGTH*2];
Int16 rcv_r[MAX_DCT_LENGTH*2], xmt_r[MAX_DCT_LENGTH*2];

Int16 pingpong = -1;
Uint16 taskList;		// bit0 -- audio processing

// fs=16000Hz
// BUFSIZE=20ms x 2channels
// HALF buffer interrupt
// every 20ms issue an interrupt
void interrupt dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	static int cnt = 50;
	if(ifrValue & 0x00f0) {
		pingpong = CSL_DMA1_REGS->DMACH0TCR2;
		taskList |= AUDIOPROCESSING;
		if(--cnt <= 0) {
			CSL_GPIO_REGS->IOOUTDATA2 ^= 0x0001;	// red LED
			cnt = 50;
		}
	}
	CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;
}

void DMA_audio_init(Uint16 size)
{
	CSL_DmaRegsOvly dma_1 = CSL_DMA1_REGS;
	CSL_SysRegsOvly regs = CSL_SYSCTRL_REGS;
	CSL_I2sRegsOvly i2s_2 = CSL_I2S2_REGS;
	Uint32 addr;

/*ADC left***********************************************************/
	// DMA1 ch0 : ADC--  i2s.left->daram
	dma_1->DMACH0TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0000 |		/* DST address inc. */
						0x0080 |		/* SRC address constant */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2 */
						0x0001;			/* Ping Pong mode */

	dma_1->DMACH0SSAL = (Uint16)&i2s_2->I2SRXLT0;
	dma_1->DMACH0SSAU = 0;
	addr = ((Uint32)rcv_l << 1) + 0x010000;		/* DARAM OFFSET = 0x010000 */
	dma_1->DMACH0DSAL = addr & 0xffff;
	dma_1->DMACH0DSAU = (Uint16)(addr >> 16);
	dma_1->DMACH0TCR1 = size;

/*DAC left***********************************************************/
	// DMA1 ch1 : DAC--  daram->i2s.left
	dma_1->DMACH1TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0200 |		/* DST address constant */
						0x0000 |		/* SRC address inc. */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2*/
						0x0001;			/* Ping Pong mode */

	addr = ((Uint32)xmt_l << 1) + 0x010000;
	dma_1->DMACH1SSAL = addr & 0xffff;
	dma_1->DMACH1SSAU = (Uint16)(addr >> 16);
	dma_1->DMACH1DSAL = (Uint16)&i2s_2->I2STXLT0;
	dma_1->DMACH1DSAU = 0;
	dma_1->DMACH1TCR1 = size;

/*ADC right**********************************************************/
	// DMA1 ch2 : ADC--  i2s.right->daram
	dma_1->DMACH2TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0000 |		/* DST address inc. */
						0x0080 |		/* SRC address constant */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2 */
						0x0001;			/* Ping Pong mode */

	dma_1->DMACH2SSAL = (Uint16)&i2s_2->I2SRXRT0;
	dma_1->DMACH2SSAU = 0;
	addr = ((Uint32)rcv_r << 1) + 0x010000;		/* DARAM OFFSET = 0x010000 */
	dma_1->DMACH2DSAL = addr & 0xffff;
	dma_1->DMACH2DSAU = (Uint16)(addr >> 16);
	dma_1->DMACH2TCR1 = size;

/*DAC right**********************************************************/
	// DMA1 ch3 : DAC--  daram->i2s.right
	dma_1->DMACH3TCR2 = 0x0000 |		/* Disable */
						0x2000 |		/* Interrupt enable */
						0x1000 |		/* auto reload */
						0x0200 |		/* DST address constant */
						0x0000 |		/* SRC address inc. */
						0x0000 |		/* burst mode bit5-3 =001 */
						0x0000 |		/* synchronized to CHnEVT bit2*/
						0x0001;			/* Ping Pong mode */

	addr = ((Uint32)xmt_r << 1) + 0x010000;
	dma_1->DMACH3SSAL = addr & 0xffff;
	dma_1->DMACH3SSAU = (Uint16)(addr >> 16);
	dma_1->DMACH3DSAL = (Uint16)&i2s_2->I2STXRT0;
	dma_1->DMACH3DSAU = 0;
	dma_1->DMACH3TCR1 = size;

	regs->DMA1CESR1 =	0x0002 |		/* I2S rcv_l */
						(0x0001 << 8);	/* I2S xmt_l */
	regs->DMA1CESR2 =	0x0002 |		/* I2S rcv_r */
						(0x0001 << 8);	/* I2S xmt_r */
	regs->DMAIER |= 0x0010;					/* DMA1 ch0 interrupt enable */

	/* Enable DMA */
	dma_1->DMACH0TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */
	dma_1->DMACH2TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */
	dma_1->DMACH1TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */
	dma_1->DMACH3TCR2 |= 0x8000 |		/* Enable */
						0x0004;			/* synchronized to CHnEVT */

	/* Enabling Interrupt */
	CSL_CPU_REGS->IER0 |= (1 << DMA_EVENT);
}

Uint16 number_of_bits_per_frame;
Uint16 number_of_bytes_per_frame;
Uint16 number_of_16bit_words_per_frame;

Uint16 gDct_length_log;
Uint16 gDct_length;
Uint16 gNumber_of_regions;
Uint16 gNum_categorization_control_bits;

Uint16 gNumber_of_valid_coefs;
Uint16 gNum_categorization_control_possibilities;

void audioProcessing(HANDLE FHandle)
{
	Int16  *samples;
	Int16  out_words[MAX_BITS_PER_FRAME/16];
	Uint8  byte16[MAX_BITS_PER_FRAME/8];
	Int16  mlt_coefs[MAX_DCT_LENGTH];

	Int16  mag_shift;
	static Int16  old_mag_shift = 0;
	Int16  offset;

	if((taskList & AUDIOPROCESSING) == 0)	return;
	offset = ((pingpong>>1) & 1) * gDct_length;

	DSP_memcpy(&xmt_r[offset], &rcv_r[offset], gDct_length);

	// Read frame of samples from mem.
	samples = &rcv_l[offset];
	// Convert input samples to rmlt coefs
	mag_shift = samples_to_rmlt_coefs(samples,
									  mlt_coefs,
									  gDct_length,
									  0);
	// Encode the mlt coefs
	encoder(number_of_bits_per_frame,
			mlt_coefs,
			mag_shift,
			out_words);

	DSP_word2byte(byte16,
				  out_words,
				  number_of_16bit_words_per_frame);

	// Write output bitstream
	if (FHandle != Not_Open_FILE)
		FileWrite(byte16, (number_of_16bit_words_per_frame << 1), FHandle);

//	DSP_byte2word(out_words,
//				  byte16,
//				  number_of_16bit_words_per_frame);
	decoder(out_words,
			mlt_coefs,
			&mag_shift,
			&old_mag_shift);

	samples = &xmt_l[offset];
	// convert the decoder_mlt_coefs to samples
	rmlt_coefs_to_samples(mlt_coefs,
						  samples, gDct_length,
						  mag_shift, 0);

	taskList &= ~AUDIOPROCESSING;
}

void Audio_init(Uint16 bandwidth)
{
	Uint8 sampleRate = 0;

	DSP_zero(xmt_l, MAX_DCT_LENGTH * 2);	// avoid reset noise!!!
	DSP_zero(xmt_r, MAX_DCT_LENGTH * 2);

	if(bandwidth == 7) {
		gDct_length_log = 6;
		sampleRate = 6;	/* fs = 16kHz */
	} else if(bandwidth == 14) {
		gDct_length_log = 7;
		sampleRate = 3;	/* fs = 32kHz */
	}

	aic3204_init(sampleRate);	/* Initialize codec, fs = 32kHz */
	gDct_length = 5 * (1<<gDct_length_log);
	gNumber_of_regions = bandwidth * 2;
	gNum_categorization_control_bits = gDct_length_log - 2;

	gNumber_of_valid_coefs = gNumber_of_regions * REGION_SIZE;
	gNum_categorization_control_possibilities =
								1<<gNum_categorization_control_bits;

	DMA_audio_init(gDct_length * 4);

	sam2coef_init();
	coef2sam_init();
	EZDSP5535_I2S_init();		/* Initialize I2S */

	number_of_bits_per_frame = (Uint16)(BITRATE * 20);	// BITRATE in kbps
	number_of_bytes_per_frame = (Uint16)(number_of_bits_per_frame>>3);
	number_of_16bit_words_per_frame = (Uint16)(number_of_bits_per_frame>>4);

	taskList &= ~AUDIOPROCESSING;
}

