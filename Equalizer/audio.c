#include "ezdsp5535.h"
#include "radio.h"
#include "c55x.h"

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

Int16 pingpong = -1;
extern Uint16 taskList;

interrupt void dma_isr(void)
{
	int ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	if(ifrValue & 0x00f0) {
		pingpong = CSL_DMA1_REGS->DMACH0TCR2;
		taskList |= AUDIOPROCESSING;
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

/*
 *                 -1      -2
 *        b0 + b1*z  + b2*z
 * H(z) = -------------------------
 *                 -1      -2 
 *         1 + a1*z  + a2*z
 *
 *   IIR Biquad Structure
 *
 *                  d(n)
 *  x(n) ---> + -----.-------- + --> y(n)
 *           /|      |    b0   |\
 *          | |    [z-1]       | |
 *          | |      |         | |
 *          | |      v         | |
 *          | |----d(n-1)------| |
 *          |  -a1   |    b1     |
 *          |        |           |
 *          |      [z-1]         |
 *          |        |           |
 *          |        v           |
 *          |------d(n-2)--------|
 *            -a2         b2
 *
 *             FB            FF
 *
 * Algorithm (for single biquad)
 * -----------------------------
 *  d(n) =    x(n) - a1*d(n-1) - a2*d(n-2)
 *  y(n) = b0*d(n) + b1*d(n-1) + b2*d(n-2)
 *
*/

/*  a2, a1 , b2, b1, b0 */
biquad filterCoeffs[EQBAND];
Int16 dbuffer_l[2*EQBAND];
Int16 dbuffer_r[2*EQBAND];

#define		FRAC	(10)		/* dbuffer Q6.10 */

Int16 biquadFilter(Int16 input, Int16 *buffer)
{
	Int16 i;
	Int16 d0;
	Int32 Ltemp1, Ltemp2;
	Int16 *coeff = (Int16 *)filterCoeffs;;

	Ltemp1 = (Int32)input << (FRAC + 1);  /* Q1.15->Q6.26 */
	for(i = 0; i < EQBAND; i++) {
		Ltemp1  = _smas(Ltemp1, *coeff++, *buffer++);
		/* Q6.26 <== Q1.15 x Q6.10 */
		Ltemp2  = _lsmpy(*coeff++, *buffer--);
		/* Q7.25 <== Q2.14 x Q6.10 */
		Ltemp1  = _lssub(Ltemp1, _lsshl(Ltemp2, 1));
		d0      = (Int16)(Ltemp1 >> 16);	/* Q6.10 */

		Ltemp1  = _lsmpy(*coeff++, *buffer++);
		/* Q10.23<== Q5.11 x Q6.10 */
		Ltemp2  = _lsmpy(*coeff++, *buffer--);
		/* Q7.25 <== Q2.14 x Q6.10 */
		Ltemp1  = _smac(Ltemp1, *coeff++, d0);
		/* Q10.23<== Q5.11 x Q6.10 */
		Ltemp1  = _lsshl(Ltemp1, 3);			/*Q7.25*/
		Ltemp1  = _lsadd(Ltemp1, Ltemp2);		/*Q7.25*/

		*buffer++ = *(buffer + 1);
		*buffer++ = d0;
	}
	Ltemp1 = _lsshl(Ltemp1, 16 - FRAC);
	return (Int16)(Ltemp1 >> 16);
}

void audioProcessing(void)
{
	Uint16  offset;
	int j;

	if((taskList & AUDIOPROCESSING) == 0)	return;
	taskList &= ~AUDIOPROCESSING;

	offset = ((pingpong>>1) & 1) * FRAMESIZE;

	for(j = 0; j < FRAMESIZE; j++) {
		xmt_l[offset + j] = biquadFilter(rcv_l[offset + j], dbuffer_l);
		xmt_r[offset + j] = biquadFilter(rcv_r[offset + j], dbuffer_r);
	}
}

