#include "ezdsp5535.h"
/* CPU clock = (multipler+4)*32768
   clock is set to 2 times of cpu clock for simple calculation
*/
void PLL_init(Uint32 clock)
{
	Uint16 regVal;

	regVal = (Uint16)(_sround(clock) >> 16);
	regVal -= 4;
	CSL_SYSCTRL_REGS->CCR2  = 0x0000;			// PLL bypass mode
	CSL_SYSCTRL_REGS->CGCR1 = 0x8000|regVal;	// (multiplier+4)*32768Hz
	CSL_SYSCTRL_REGS->CGICR = 0x8000;			// bypass the reference divider
	CSL_SYSCTRL_REGS->CGCR2 = 0x0806;			// INIT, must be 0x0806
	CSL_SYSCTRL_REGS->CGOCR = 0x0000;			// ODRATIO = 1
	do {
		regVal = CSL_SYSCTRL_REGS->CGCR2;
	} while ((regVal & 8) == 0);

	CSL_SYSCTRL_REGS->CCR2  = 0x0001;			// PLL mode set
}

