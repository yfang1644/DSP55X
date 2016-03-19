#include "ezdsp5535.h"

// 0xbe8+4, 100.007936MHz
// 0x723+4, 59.998208MHz
// 0xe4a+4, 119.996416MHz

void PLL_init(Uint16 multipler)
{
	CSL_SYSCTRL_REGS->CCR2  = 0x0000;			// PLL bypass mode
	CSL_SYSCTRL_REGS->CGCR1 = 0x8000|multipler;	// (multiplier+4)*32768Hz
	CSL_SYSCTRL_REGS->CGICR = 0x8000;			// bypass the reference divider
	CSL_SYSCTRL_REGS->CGCR2 = 0x0806;			// INIT, must be 0x0806
	CSL_SYSCTRL_REGS->CGOCR = 0x0000;			// ODRATIO = 1
	EZDSP5535_waitusec(10);						// lock time, at least 4ms
	CSL_SYSCTRL_REGS->CCR2  = 0x0001;			// PLL mode set
}

