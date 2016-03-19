//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_i2s.c
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"
/*  comb-filter, eliminate 50Hz
 *                 -N
 *            1 - z                 1 + r
 *H(z) = b  --------------   , b = -------
 *                   -N               2
 *            1 - r z
 *
 * y(n) = 0.8*(x(n) - x(n-T)) + 0.6*y(n-T);
 * y(n) = 0.8*x(n) + [0.6*y(n-T) - 0.8*x(n-T)]
 *                   |-----------------------| delaybuffer
 *
 *  r = 0.94(30802), b=0.97(31785)
 */

extern Int16 period;
Int16 delay_array[960];		// minimum frequency=50Hz @48kHz sampling

Int16 dataL, dataR;

interrupt void i2s2_tx_isr(void)
{
	CSL_I2S2_REGS->I2STXLT1 = dataL;
	CSL_I2S2_REGS->I2STXRT1 = dataR;
}

interrupt void i2s2_rx_isr(void)
{
	static int index = 0;
	Int16 x0, y0;

	dataL = CSL_I2S2_REGS->I2SRXLT1;
	dataR = CSL_I2S2_REGS->I2SRXRT1;

	x0 = _smpy(31785, dataL);
	y0 = _sadd(x0, delay_array[index]);
	dataL = y0;
	y0 = _smpy(30802, y0);
	y0 = _ssub(y0, x0);
	delay_array[index] = y0;  /* Overwrite buffer with new value */

	if ( ++index >= period)
		index = 0;
}

/*
 *  EZDSP5535_I2S_init(int mode)
 *      mode = 1: interrupt mode
 *      mode = 0: DMA mode
 *
 *      Initialize I2S module.
 */
void EZDSP5535_I2S_init(int mode)
{
	CSL_I2sRegsOvly regs = CSL_I2S2_REGS; 

	regs->I2SSRATE = 0;
	regs->I2SSCTRL = 0x0000 |		/* stereo bit 12=0 */
		0x0000 |		/* loopback disable bit 11=0 */
		0x0000 |		/* frame sync. low bit 10=0 */
		0x0000 |		/* rising edge bit 9=0 */
		0x0000 |		/* data delay 1 bit 8=0 */
		0x0080 |		/* data pack enable bit 7=1 */
		0x0000 |		/* sign ext. disable bit 6=0 */
		0x0010 |		/* wordlength 16bit bit 5-2=0100 */
		0x0000 |		/* slave bit 1=0 */
		0x0000;		/* bit0=0 */

	regs->I2SINTMASK = 0x0028;	/* rcv and xmt interrupt only */
	regs->I2SSCTRL |= 0x8000;	/* trans enable bit15=1 */

	/* Configuring Interrupt */
	if(mode == 0) {
		CSL_CPU_REGS->IER0 &= ~0xc000;
	} else {
		CSL_CPU_REGS->IER0 |= 0xc000;
	}
}

