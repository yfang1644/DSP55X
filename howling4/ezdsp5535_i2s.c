//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_i2s.c
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"

extern Int16 rcv_l[], rcv_r[];
Uint16 sample_ptr = 0;
Uint16 delay = 0;

interrupt void i2s2_tx_isr(void)
{
}

interrupt void i2s2_rx_isr(void)
{
	static int cnt = 0;
	Uint16 ptr = sample_ptr - delay;
	if(cnt++ > 8000)
	{
		CSL_GPIO_REGS->IOOUTDATA1 ^= 0x8000;
		cnt = 0;
	}
	rcv_l[sample_ptr] = CSL_I2S2_REGS->I2SRXLT1;
	rcv_r[sample_ptr] = CSL_I2S2_REGS->I2SRXRT1;
	ptr &= 0x1fff;

	CSL_I2S2_REGS->I2STXLT1 = rcv_l[ptr];
	CSL_I2S2_REGS->I2STXRT1 = rcv_r[ptr];
	sample_ptr++;
	sample_ptr &= 0x1fff;
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

