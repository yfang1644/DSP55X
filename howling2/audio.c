#include "ezdsp5535.h"
#include "radio.h"

// DMA1 channel 0 for I2S receive
// DMA1 channel 1 for I2S transmit
#pragma DATA_SECTION(rcv_l, "dmaMem")
#pragma DATA_SECTION(xmt_l, "dmaMem")
#pragma DATA_SECTION(rcv_r, "dmaMem")
#pragma DATA_SECTION(xmt_r, "dmaMem")
#pragma DATA_ALIGN (rcv_l, 4)
#pragma DATA_ALIGN (xmt_l, 4)
#pragma DATA_ALIGN (rcv_r, 4)
#pragma DATA_ALIGN (xmt_r, 4)
Int16 rcv_l[FFT_L*2], xmt_l[FFT_L*2];
Int16 rcv_r[FFT_L*2], xmt_r[FFT_L*2];

#pragma DATA_SECTION(fftx, ".input")
#pragma DATA_SECTION(ffty, ".input")
Complex fftx[FFT_L];
Complex ffty[FFT_L];

// rcv_l[]  primary channel (refer to chn0) -- adaptive filter
// rcv_r[]  reference channel (refer to chn1)
// primary channel = signal + noise
// reference channel = only correlated noise
// error = chn0 * w - chn1
// FFT(error) - FFT(chn1)
// 
// xmt_l[]  direct copy from rcv_l
// xmt_r[]  processed

Int16 pingpong = -1;
Uint16 taskList = 0;		// bit0 -- audio processing

// fs=16000Hz
// BUFSIZE=512x2channels
// HALF buffer interrupt
// every 512/16000/2 seconds issue an interrupt

interrupt void dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	static int cnt = 50;
	if(ifrValue & 0x00f0) {
		pingpong = CSL_DMA1_REGS->DMACH0TCR2;
		taskList |= AUDIOPROCESSING;
		if(--cnt <= 0)
		{
			CSL_GPIO_REGS->IOOUTDATA1 ^= 0x4000;	// red LED
			cnt = 20;
		}
	}

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
	CSL_CPU_REGS->IER0 |= 0x0100;
}

void audioProcessing(int func)
{
	Int16  offset;
	int i;

	if((taskList & AUDIOPROCESSING) == 0)	return;

	taskList &= ~AUDIOPROCESSING;

	offset = ((pingpong>>1) & 1) * FFT_L;
	if(func == 0) {
		DSP_memcpy(&xmt_l[offset], &rcv_l[offset], FFT_L);
		DSP_memcpy(&xmt_r[offset], &rcv_r[offset], FFT_L);
		return;
	}

	for(i = 0; i < FFT_L; i++) {
		ffty[i].re = rcv_l[offset + i];
		ffty[i].im = 0;
	}
	cfft32_NOSCALE(ffty, FFT_L);					// FFT
	cbrev32(ffty, fftx, FFT_L);

	for(i = 1; i < FFT_L/2; i++) {
		fftx[i].re = fftx[i + 1].re;
		fftx[i].im = -fftx[i + 1].im;

		fftx[FFT_L - i].re = fftx[i].re;
		fftx[FFT_L - i].im = -fftx[i].im;
	}
	fftx[0].re = 0; fftx[0].im = 0;

	cfft32_NOSCALE(fftx, FFT_L);					// IFFT
	cbrev32(fftx, ffty, FFT_L);

	DSP_restore(ffty, &xmt_r[offset], FFT_L, M);
	DSP_memcpy(&xmt_l[offset], &xmt_r[offset], FFT_L);
}

