#include "soc.h"

extern void Reset();
void INTR_init(void)
{
	CSL_CPU_REGS->IVPD = (unsigned long)Reset >> 8;
	CSL_CPU_REGS->IVPH = (unsigned long)Reset >> 8;
	CSL_CPU_REGS->IER0 = 0x0000;
	CSL_CPU_REGS->IER1 = 0x0000;
	CSL_CPU_REGS->IFR0 = 0xffff;
	CSL_CPU_REGS->IFR1 = 0xffff;

	CSL_SYSCTRL_REGS->DMAIER = 0x0000;
	CSL_SYSCTRL_REGS->DMAIFR = 0xffff;
}

