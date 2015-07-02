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
Int16 rcv_l[FRAMESIZE*2], rcv_r[FRAMESIZE*2];
Int16 xmt_l[FRAMESIZE*2], xmt_r[FRAMESIZE*2];

#define		BUFFSIZE	8192

#pragma DATA_ALIGN (l_buffer, 4)
#pragma DATA_ALIGN (r_buffer, 4)
Int16 l_buffer[BUFFSIZE];		// 延迟缓冲区, 48kHz 约 170ms
Int16 r_buffer[BUFFSIZE];		// 延迟缓冲区, 48kHz 约 170ms

Int16 pingpong = -1;
Uint16 taskList = 0;

interrupt void dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	static int cnt = 0;
	if(ifrValue & 0x00f0) {
		pingpong = CSL_DMA1_REGS->DMACH0TCR2;
		taskList |= AUDIOPROCESSING;
		if(++cnt >= 200) {
			CSL_GPIO_REGS->IOOUTDATA1 ^= 0x4000;    // blue LED
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

#define		ITEMS		23
Int16 revidx[ITEMS][3] = {
	{  -190,  -190, 0x6BA6},    // 0.841
	{  -949,  -949, 0x4083},    // 0.504
	{  -993,  -993, 0x3ED9},    // 0.490
	{ -1183, -1183, -0x3083},    // 0.379
	{ -1192, -1192, 0x30A4},    // 0.380
	{ -1315, -1315, 0x2C4A},    // 0.346
	{ -2021, -2021, 0x24FE},    // 0.289
	{ -2140, -2140, -0x22D1},    // 0.272
	{ -2524, -2524, -0x1893},    // 0.192
	{ -2590, -2590, 0x18B4},    // 0.193
	{ -2625, -2625, 0x1BC7},    // 0.217
	{ -2700, -2700, 0x172B},    // 0.181
	{ -3119, -3119, -0X170A},    // 0.180
	{ -3123, -3123, 0x172B},    // 0.181
	{ -3202, -3202, 0x1687},    // 0.176
	{ -3268, -3268, -0x122D},    // 0.142
	{ -3321, -3321, 0x1560},    // 0.167
	{ -1000, -1000, 0x2000},
	{ -2000, -2000, 0x2000},
	{ -3000, -3000, 0x4000},
	{ -4000, -4000, 0x6000},
	{ -5000, -5000, 0x3000},
	{ -3515, -3515, 0x1127}     // 0.134
};

void audioProcessing(Uint16 state)
{
	Uint16  offset;
	static Uint16 bufptr = 0;
	Uint16 i, lptr, tap;
	Int32 Ltemp;

	if((taskList & AUDIOPROCESSING) == 0)	return;
	taskList &= ~AUDIOPROCESSING;

	offset = ((pingpong>>1) & 1) * FRAMESIZE;

	DSP_memcpy(&l_buffer[bufptr], &rcv_l[offset], FRAMESIZE);
	DSP_memcpy(&r_buffer[bufptr], &rcv_r[offset], FRAMESIZE);

	if(state & 1) {			// left
		for(i = 0; i < FRAMESIZE; i++) {
			Ltemp = (Uint32)rcv_l[offset + i] << 16;
			for(tap = 0; tap < ITEMS; tap++) {
				lptr = (revidx[tap][0]++) & (BUFFSIZE - 1);
				Ltemp = _smac(Ltemp, l_buffer[lptr], revidx[tap][2]);
			}
			xmt_l[offset + i] = (Int16)(Ltemp >> 16);
		}
	} else {
		DSP_memcpy(&xmt_l[offset], &rcv_l[offset], FRAMESIZE);
		for(tap = 0; tap < ITEMS; tap++)
			revidx[tap][0] += FRAMESIZE;
	}

	if(state & 2) {			// right
		for(i = 0; i < FRAMESIZE; i++) {
			Ltemp = (Uint32)rcv_r[offset + i] << 16;
			for(tap = 0; tap < ITEMS; tap++) {
				lptr = (revidx[tap][1]++) & (BUFFSIZE - 1);
				Ltemp = _smac(Ltemp, r_buffer[lptr], revidx[tap][2]);
			}
			xmt_r[offset + i] = (Int16)(Ltemp >> 16);
		}
	} else {
		DSP_memcpy(&xmt_r[offset], &rcv_r[offset], FRAMESIZE);
		for(tap = 0; tap < ITEMS; tap++)
			revidx[tap][1] += FRAMESIZE;
	}

	bufptr = (bufptr + FRAMESIZE) & (BUFFSIZE-1);
}

