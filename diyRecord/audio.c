#include "ezdsp5535.h"
#include "diyrecord.h"
#include "libg722_1.h"
#include "fileops.h"

// DMA1 channel 0 for I2S receive
// DMA1 channel 1 for I2S transmit
#pragma DATA_SECTION(rcv_l, ".cio")
#pragma DATA_SECTION(xmt_l, ".cio")
#pragma DATA_SECTION(rcv_r, ".cio")
#pragma DATA_SECTION(xmt_r, ".cio")
#pragma DATA_SECTION(mix_lr, ".cio")
#pragma DATA_ALIGN (rcv_l, 4)
#pragma DATA_ALIGN (xmt_l, 4)
#pragma DATA_ALIGN (rcv_r, 4)
#pragma DATA_ALIGN (xmt_r, 4)
#pragma DATA_ALIGN (mix_lr, 4)
Int16 rcv_l[MAX_DCT_LENGTH*2], xmt_l[MAX_DCT_LENGTH*2];
Int16 rcv_r[MAX_DCT_LENGTH*2], xmt_r[MAX_DCT_LENGTH*2];
Uint8 mix_lr[MAX_DCT_LENGTH*4];					// 0l0l0l0l....0r0r0r0r

Int16 *inputDmaChn[] = {rcv_l, rcv_r};
Int16 *outputDmaChn[] = {xmt_l, xmt_r};

Uint8 *wrFilePtrl = &mix_lr[0],
	  *wrFilePtrr = &mix_lr[MAX_DCT_LENGTH*2];

unsigned short WATERMARK = WATERMARK_CODE;
Int16 pingpong = -1;
extern Uint16 taskList;		// bit0 -- audio processing
extern recordStruct recordFile;

// fs=16000Hz
// BUFSIZE=512x2channels
// HALF buffer interrupt
// every 512/16000/2 seconds issue a interrupt
volatile Uint16 dmaIsrStatus = 0;
void interrupt dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	static int cnt = 50;
	if(ifrValue & 0x00f0) {
		pingpong = CSL_DMA1_REGS->DMACH0TCR2;
		taskList |= AUDIOPROCESSING;
		if(--cnt <= 0)
		{
			CSL_GPIO_REGS->IOOUTDATA2 ^= 0x0001;	// red LED
			cnt = 50;
		}
	}
	dmaIsrStatus |= ifrValue;
	CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;
}

// DMA1 channel 0 for I2S receive left
// DMA1 channel 1 for I2S transmit left
// DMA1 channel 2 for I2S receive right
// DMA1 channel 3 for I2S transmit right
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
	dma_1->DMACH0TCR1 = 4 * size;

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
	dma_1->DMACH1TCR1 = 4 * size;

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
	dma_1->DMACH2TCR1 = 4 * size;

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
	dma_1->DMACH3TCR1 = 4 * size;

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
	CSL_CPU_REGS->IER0 |= 0x0100;
}

Int16  rawOutWords[2*(MAX_BITS_PER_FRAME/16+1)] = {0x6b21};
Uint8  RfByte[2*(MAX_BITS_PER_FRAME/8+2)] = {0x6b, 0x21};		// left and right

Uint16 number_of_bits_per_frame;
Uint16 number_of_bytes_per_frame;
Uint16 number_of_16bit_words_per_frame;

Uint16 gDct_length_log;
Uint16 gDct_length;
Uint16 gNumber_of_regions;
Uint16 gNum_categorization_control_bits;

Uint16 gNumber_of_valid_coefs;
Uint16 gNum_categorization_control_possibilities;

void audioProcessing(void)
{
	Int16  *samples;
	Int16  *out_words;
//	Uint8  *byte16 = &RfByte[2];
	Int16  mlt_coefs[MAX_DCT_LENGTH];
	Int16  mag_shift[2];
	static Int16  old_mag_shift[2] = {0, 0};
	Int16  offset;
	Uint16 chn;		// left and right channel, 0 or 1
	static Uint16 ledtgl = 0, mark = 0;    // toggle LED mark

	if((taskList & AUDIOPROCESSING) == 0)	return;
	offset = ((pingpong>>1) & 1) * gDct_length;

	switch(recordFile.function) {
	case DIRECT_IO:
		DSP_memcpy(&xmt_l[offset], &rcv_l[offset], gDct_length);
		DSP_memcpy(&xmt_r[offset], &rcv_r[offset], gDct_length);
		break;

	case FILE_RECORD_RAW:
		for(chn = 0; chn < STEREO; chn++) {
			samples = inputDmaChn[chn] + offset;	// Read data from DMA buffer
			if(recordFile.channel & (1 << chn)) {	// RECORD chn
				DSP_watermark(&mix_lr[2 * MAX_DCT_LENGTH * chn],
							  samples, gDct_length);
			}
			DSP_memcpy(outputDmaChn[chn] + offset, samples, gDct_length);
		}
		break;

	case FILE_RECORD_COD1:
		for(chn = 0; chn < STEREO; chn++) {
			samples = inputDmaChn[chn] + offset;	// Read data from DMA buffer
			if(recordFile.channel & (1 << chn)) { // RECORD chn
			// Convert input samples to rmlt coefs
				mag_shift[chn] = samples_to_rmlt_coefs(samples,
													   mlt_coefs,
													   gDct_length,
													   chn);
			// Encode the mlt coefs
				out_words = &rawOutWords[1];
				encoder(number_of_bits_per_frame,
						mlt_coefs,
						mag_shift[chn],
						out_words);
				DSP_word2byte(&mix_lr[2 * MAX_DCT_LENGTH * chn],
							  (Uint16 *)out_words,
							  number_of_16bit_words_per_frame);
			}
			DSP_memcpy(outputDmaChn[chn] + offset, samples, gDct_length);
		}
		break;

	case FILE_REPLAY_RAW:
		for(chn = 0; chn < STEREO; chn++) {
			samples = outputDmaChn[chn] + offset;	// write data to DMA buffer
			if(recordFile.channel & (1 << chn)) {	// REPLAY chn
				DSP_byte2word((Uint16 *)samples,
							  &mix_lr[2 * MAX_DCT_LENGTH * chn],
							  gDct_length);
			} else									// direct in->out
				DSP_memcpy(samples, inputDmaChn[chn] + offset, gDct_length);
		}
		break;

	case FILE_REPLAY_COD1:
		for(chn = 0; chn < STEREO; chn++) {
			samples = outputDmaChn[chn] + offset;	// Write data to DMA buffer
			if(recordFile.channel & (1 << chn)) {	// REPLAY chn
			// process the out_words into decoder_mlt_coefs
				DSP_byte2word((Uint16 *)out_words,
							  &mix_lr[2 * MAX_DCT_LENGTH * chn],
							  number_of_16bit_words_per_frame);
				decoder(out_words,
						mlt_coefs,
						&mag_shift[chn],
						&old_mag_shift[chn]);

			// convert the decoder_mlt_coefs to samples
				rmlt_coefs_to_samples(mlt_coefs,
									  samples, DCT_LENGTH,
									  mag_shift[chn], chn);
			} else
				DSP_memcpy(samples, inputDmaChn[chn] + offset, gDct_length);
		}
		break;
		
	default:
		break;
	}

	if(ledtgl++ > 10) {
		ledtgl = 0;
		mark = 1 - mark;
		if(mark)	asm("	bit(ST1, #ST1_XF) = #1");
		else		asm("	bit(ST1, #ST1_XF) = #0");
	}

	taskList &= ~AUDIOPROCESSING;
	taskList |= FILEPROCESSING;				// may need fileProcessing()
}

void Audio_init(Uint16 bandwidth)
{
	DSP_zero(xmt_l, MAX_DCT_LENGTH);		// avoid reset noise!!!
	DSP_zero(xmt_r, MAX_DCT_LENGTH);

	if(bandwidth == 7) {
		gDct_length_log = 6;
		recordFile.sampleRate = 6;	/* fs = 16kHz */
	} else if(bandwidth == 14) {
		gDct_length_log = 7;
		recordFile.sampleRate = 3;	/* fs = 32kHz */
	}

	aic3204_init(recordFile.sampleRate, 0);	/* Initialize codec, fs = 32kHz */
	gDct_length = 5 * (1<<gDct_length_log);
	gNumber_of_regions = bandwidth * 2;
	gNum_categorization_control_bits = gDct_length_log - 2;

	gNumber_of_valid_coefs = gNumber_of_regions * REGION_SIZE;
	gNum_categorization_control_possibilities =
								1<<gNum_categorization_control_bits;

	DMA_audio_init(gDct_length);

	sam2coef_init();
	coef2sam_init();
	EZDSP5535_I2S_init();		/* Initialize I2S */

	number_of_bits_per_frame = (Uint16)(BITRATE * 20);	// BITRATE in kbps
	number_of_bytes_per_frame = (Uint16)(number_of_bits_per_frame>>3);
	number_of_16bit_words_per_frame = (Uint16)(number_of_bits_per_frame>>4);
	taskList &= ~AUDIOPROCESSING;
}

