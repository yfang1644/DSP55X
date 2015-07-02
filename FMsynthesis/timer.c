#include "ezdsp5535_sar.h"
#include "radio.h"

extern Uint16 taskList;

interrupt void tim_isr(void)
{
	CSL_SYSCTRL_REGS->TIAFR = 0x01;
	CSL_GPIO_REGS->IOOUTDATA2 ^= 0x0003;    // red LED
	taskList |= MUSICPROCESSING;
}

void TIMER_init(Uint32 period)
{
//#define	TIME_CONST	29999104L	// 119.99MHz/4
//#define	TIME_CONST	25001984L	// 100.007936MHz/4
	CSL_TIM_0_REGS->TIMPRD1 = period & 0xffff;
	CSL_TIM_0_REGS->TIMPRD2 = period >> 16;
	// prescale 0000=2
	CSL_TIM_0_REGS->TCR = 0x8001;

	/* Enabling Interrupt */
	CSL_CPU_REGS->IER0 |= 0x0010;		// interrupt every 1 second

	CSL_SYSCTRL_REGS->TIAFR = 0x01;
}

