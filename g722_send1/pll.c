#include "ezdsp5535.h"

void PLL_init()
{
	CSL_SYSCTRL_REGS->CCR2  = 0x0000;			// PLL bypass mode
//	CSL_SYSCTRL_REGS->CGCR1 = 0x8be7;			// multiplier 0xbe7+4,  99.975168MHz
//	CSL_SYSCTRL_REGS->CGCR1 = 0x8724;			// multiplier 0x724+4,  60MHz
	CSL_SYSCTRL_REGS->CGCR1 = 0x8e4a;			// multiplier 0xe4a+4, 119.996416MHz
	CSL_SYSCTRL_REGS->CGICR = 0x8000;			// bypass the reference divider
	CSL_SYSCTRL_REGS->CGCR2 = 0x0806;			// INIT, must be 0x0806
	CSL_SYSCTRL_REGS->CGOCR = 0x0000;			// ODRATIO = 1
	EZDSP5535_waitusec(10);						// lock time, at least 4ms
	CSL_SYSCTRL_REGS->CCR2  = 0x0001;			// PLL mode set
	// system clock set to 99.975168MHz
	//					119.996416
}

