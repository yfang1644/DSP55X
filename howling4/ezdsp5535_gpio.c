#include "ezdsp5535_gpio.h"

void EZDSP5535_GPIO_init()
{
	CSL_GPIO_REGS->IODIR1 = 0xc000;			// for LED blue & yellow
	CSL_GPIO_REGS->IODIR2 = 0x0003;			// for LED red & green
	CSL_GPIO_REGS->IOOUTDATA1 = 0xc000;
	CSL_GPIO_REGS->IOOUTDATA2 = 0x0003;
}

