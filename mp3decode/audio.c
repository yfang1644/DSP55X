#include "ezdsp5535.h"
#include "defs.h"
#include "audio.h"
#include "ff.h"

#include "decoder.h"

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
Int16 rcv_l[MAX_BUFFER_SIZE*2], xmt_l[MAX_BUFFER_SIZE*2];
Int16 rcv_r[MAX_BUFFER_SIZE*2], xmt_r[MAX_BUFFER_SIZE*2];

volatile Int16 pingpong = -1;
Uint16 gBufferSize = 0;

// fs=16000Hz
// BUFSIZE=20ms x 2channels
// HALF buffer interrupt
// every 20ms issue an interrupt
volatile Uint16 dmaIsrStatus = 0;
void interrupt dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	static int cnt = 50;
	if(ifrValue & 0x00f0) {
		pingpong = CSL_DMA1_REGS->DMACH0TCR2 & 2;
		if(--cnt <= 0)
		{
			CSL_GPIO_REGS->IOOUTDATA2 ^= 0x0001;	// red LED
			cnt = 50;
		}
	}
	dmaIsrStatus |= ifrValue;
	CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;
}

int DMA_audio_init(Uint16 size)
{
	CSL_DmaRegsOvly dma_1 = CSL_DMA1_REGS;
	CSL_SysRegsOvly regs = CSL_SYSCTRL_REGS;
	CSL_I2sRegsOvly i2s_2 = CSL_I2S2_REGS;
	Uint32 addr;

	if(gBufferSize == size)
		return -1;

	gBufferSize = size;
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
	return 0;
}

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */
extern FIL file;
#pragma	DATA_ALIGN(readbuf, 4)

#define	BUF_SIZE	4096
unsigned char readbuf[2*BUF_SIZE];

enum mad_flow input(void *data, struct mad_stream *stream)
{
	unsigned char *buffer = data;
	int length;
	int i;
    int bytes_to_preserve = stream->bufend - stream->next_frame;

    if (bytes_to_preserve) {	
		for(i = 0; i < bytes_to_preserve; i++)
			buffer[i] = stream->next_frame[i];
    }

	length = f_read(&file, buffer + bytes_to_preserve, BUF_SIZE);
    if (length <= 0) {
		return MAD_FLOW_STOP;
    } else {
        mad_stream_buffer(stream, buffer, length+bytes_to_preserve);
		return MAD_FLOW_CONTINUE;
    }
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */

short scale(mad_fixed_t sample)
{
  /* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

  /* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */

enum mad_flow output(void *data,
		     struct mad_header const *header,
		     struct mad_pcm *pcm)
{
	unsigned short nchannels, nsamples;
	unsigned int offset;
	mad_fixed_t const *left_ch, *right_ch;

	while(pingpong < 0)	  ;
  /* pcm->samplerate contains the sampling frequency */

	nchannels = pcm->channels;
	nsamples  = pcm->length;
	left_ch   = pcm->samples[0];
	right_ch  = pcm->samples[1];

	offset = (pingpong >> 1) * gBufferSize;

	DSP_scale(&xmt_l[offset], left_ch, nsamples);
	if(nchannels == 2)
		DSP_scale(&xmt_r[offset], right_ch, nsamples);

	pingpong = -1;

	return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

enum mad_flow error(void *data,
		    struct mad_stream *stream,
		    struct mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}


struct mad_decoder decoder;
int audioProcessing(void)
{
	return mad_decoder_run(&decoder);
}

void Audio_init(void)
{
	aic3204_init(3, 0);	/* Initialize codec, fs = 32kHz */

	DMA_audio_init(1152);

	EZDSP5535_I2S_init();		/* Initialize I2S */

  /* decoder init */
	mad_decoder_init(&decoder, readbuf,
		input,
		0 /* header */,
		0 /* filter */,
		output,
		error,
		0 /* message */);
}

void clearBuffer()
{
	DSP_zero(xmt_r, MAX_BUFFER_SIZE*2);
	DSP_zero(xmt_l, MAX_BUFFER_SIZE*2);
}
