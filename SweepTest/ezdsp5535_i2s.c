//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_i2s.c
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"

#pragma DATA_SECTION(rcvbuf, ".buffer")
Int16 rcvbuf[2048];

extern Int16 sine_table[];
unsigned int delta;

interrupt void i2s2_tx_isr(void)
{
	static unsigned int sin_ptr = 0;
	CSL_I2S2_REGS->I2STXLT1 = sine_table[sin_ptr];
	CSL_I2S2_REGS->I2STXRT1 = sine_table[sin_ptr];

	sin_ptr += delta;
	sin_ptr &= (16384 - 1);
}

Int16 gMax = 0, gMin = 0;
Uint16 gBufPtr = 0;
interrupt void i2s2_rx_isr(void)
{
	Int16 val;
	val = CSL_I2S2_REGS->I2SRXLT1;
	val = CSL_I2S2_REGS->I2SRXRT1;
	if(val > gMax)
		gMax = val;
	if(val < gMin)
		gMin = val;
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
