#include "ezdsp5535_gpio.h"

void EZDSP5535_GPIO_init()
{
	CSL_GPIO_REGS->IODIR1 = 0xc000;			// for LED blue & yellow
	CSL_GPIO_REGS->IODIR2 = 0x0003;			// for LED red & green
	CSL_GPIO_REGS->IOOUTDATA1 = 0xc000;
	CSL_GPIO_REGS->IOOUTDATA2 = 0x0003;

	CSL_GPIO_REGS->IOINTEN1 = 0x0800;		// GPIO11 from RFcard
	CSL_GPIO_REGS->IODIR1 &= ~0x0800;		// GPIO input
	CSL_GPIO_REGS->IOINTEDG1 = 0x0800;		// falling edge trigger
	CSL_GPIO_REGS->IOINTFLG1 = 0x0800;

	CSL_GPIO_REGS->IODIR1 |= 0x0400;		// GPIO10, RadioIC, power down up
	CSL_GPIO_REGS->IOOUTDATA1 &= ~0x0400;
	EZDSP5535_waitusec(500);
	CSL_GPIO_REGS->IOOUTDATA1 |= 0x0400;
	EZDSP5535_waitusec(5000);
	CSL_GPIO_REGS->IOOUTDATA1 &= ~0x0400;

	CSL_CPU_REGS->IER1 |= 0x0020;			// GPIO interrupt
}

extern Int16 gpioIsrStatus;

interrupt void gpio_isr(void)
{
	static Int16 gpioCnt = 50;
	if(gpioCnt-- <= 0) {
		gpioCnt = 50;
		CSL_GPIO_REGS->IOOUTDATA1 ^= 0x8000;		// yellow LED
	}
	gpioIsrStatus = 1;
	CSL_GPIO_REGS->IOINTFLG1 |= 0x0800;
}

