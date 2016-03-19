#include "ezdsp5535_gpio.h"

void EZDSP5535_GPIO_init()
{
	CSL_GPIO_REGS->IODIR1 |= 0x8000;		// GPIO15, codec
	CSL_GPIO_REGS->IOOUTDATA1 &= ~0x8000;
	EZDSP5535_waitusec(50);
	CSL_GPIO_REGS->IOOUTDATA1 |= 0x8000;

	CSL_GPIO_REGS->IOINTEN2 = 0x0002;		// GPIO17 from RadioIC interrupt
	CSL_GPIO_REGS->IODIR2 &= ~0x0002;		// GPIO input
	CSL_GPIO_REGS->IOINTEDG2 = 0x0002;		// falling edge trigger
	CSL_GPIO_REGS->IOINTFLG2 = 0x0002;

	CSL_GPIO_REGS->IODIR1 |= 0x4000;		// GPIO10, RadioIC, power down up
	CSL_GPIO_REGS->IOOUTDATA1 &= ~0x4000;
	EZDSP5535_waitusec(50);
	CSL_GPIO_REGS->IOOUTDATA1 |= 0x4000;
	EZDSP5535_waitusec(50);
	CSL_GPIO_REGS->IOOUTDATA1 &= ~0x4000;

	CSL_CPU_REGS->IER1 |= 0x0020;			// GPIO interrupt
}

extern Int16 gpioIsrStatus;

interrupt void gpio_isr(void)
{
	static Int16 gpioCnt = 50;
	if(gpioCnt-- <= 0) {
		gpioCnt = 50;
	}
	gpioIsrStatus = 1;
	CSL_GPIO_REGS->IOINTFLG2 |= 0x0002;
}

