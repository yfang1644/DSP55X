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
Int16 rcv_l[BUFFER*2], rcv_r[BUFFER*2];
Int16 xmt_l[BUFFER*2], xmt_r[BUFFER*2];

Int16 pingpong = -1;
Uint16 taskList = 0;		// bit0 -- audio processing

interrupt void dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	static int cnt = 0;
	if(ifrValue & 0x00f0) {
		pingpong = CSL_DMA1_REGS->DMACH0TCR2;
		taskList |= AUDIOPROCESSING;
		if(++cnt >= 200)
		{
			CSL_GPIO_REGS->IOOUTDATA1 ^= 0x4000;	// blue LED
			cnt = 0;
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
	volatile Uint32 addr;

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

#define TBLNUM          12      /* sine table size = 2^12 */
#define SINETBLSIZE     (1 << TBLNUM)       /* 1/4 of one sine cycle */
Uint16 delta = 0;
extern Int16 sine_table[];
Uint16 sinPtr = 0;

void audioProcessing(int func)
{
	Uint16  offset;
	short ch1, ch2;
	Uint16 quad, ptr;
	int i;
	int c, s;       // cos, sin
    long sum;

	if((taskList & AUDIOPROCESSING) == 0)   return;

	taskList &= ~AUDIOPROCESSING;

	offset = ((pingpong>>1) & 1) * BUFFER;
	if(func == 0) {
		DSP_memcpy(&xmt_l[offset], &rcv_l[offset], BUFFER);
		DSP_memcpy(&xmt_r[offset], &rcv_r[offset], BUFFER);
		return;
	}

	for(i = 0; i < BUFFER; i++) {
		sinPtr &= (4*SINETBLSIZE-1);
		quad = sinPtr >> TBLNUM;
		ptr = sinPtr & (SINETBLSIZE-1);
		switch(quad) {
		case 0:
			s = sine_table[ptr];
			c = (ptr == 0) ? 0x7fff : sine_table[SINETBLSIZE - ptr];
			break;
		case 1:
			s = (ptr == 0) ? 0x7fff : sine_table[SINETBLSIZE - ptr];
			c = -sine_table[ptr];
			break;
		case 2:
			s = -sine_table[ptr];
			c = (ptr == 0) ? 0x8000 : -sine_table[SINETBLSIZE - ptr];
			break;
		case 3:
			s = (ptr == 0) ? 0x8000 : -sine_table[SINETBLSIZE - ptr];
			c = sine_table[ptr];
			break;
		default:
			break;
		}

		ch1 = rcv_l[offset + i];
		ch2 = rcv_r[offset + i];
		sum  = (long)ch1 * s;
		sum += (long)ch2 * c;
		ch1 = (short)(sum >> 15);
		xmt_l[offset + i] = ch1;
		xmt_r[offset + i] = ch1;
		sinPtr += delta*4;
	}
}

