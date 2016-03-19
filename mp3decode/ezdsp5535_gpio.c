#include "ezdsp5535_gpio.h"

void EZDSP5535_GPIO_init()
{
	CSL_GPIO_REGS->IODIR1 = 0xc000;			// for LED yellow & blue
	CSL_GPIO_REGS->IODIR2 = 0x0003;			// for LED green & red
	CSL_GPIO_REGS->IOOUTDATA1 = 0xc000;
	CSL_GPIO_REGS->IOOUTDATA2 = 0x0003;

//	CSL_CPU_REGS->IER1 |= 0x0020;			// GPIO interrupt
}

void interrupt gpio_isr(void)
{
}

