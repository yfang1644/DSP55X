#include "ezdsp5535.h"

extern Uint16 delta;
extern Uint32 f20;
extern int sweepDone;
extern Uint16 gBufPtr;
extern Int16 gMax, gMin;
extern Int16 rcvbuf[];

void interrupt tim_isr(void)
{
/*
	delta = 16384/48000*f
	1.0093^500 = 102.36
	20Hz - 2000Hz

	delta(20Hz) = 6.826, delta(2kHz) = 682.6
	delta = delta * (1+p);
	Q6.24 delta(20Hz)=6.826*64, p = 0.0093(Q14=0.0093*32768)
*/
	int temp;
	temp = f20 >> 16;
	f20 = f20 + _lmpy(temp, 610);
	delta = _lshrs(f20, 16);
	CSL_SYSCTRL_REGS->TIAFR = 0x01;
	rcvbuf[gBufPtr++] = (gMax - gMin) / 2;
	gMax = gMin = 0;
	if(delta > 682) {
		sweepDone = 1;
		CSL_CPU_REGS->IER0 &= ~0xc000;
		CSL_CPU_REGS->IER0 &= ~0x0010;
	}
}

void TIMER_init(Uint32 period)
{
	CSL_TIM_0_REGS->TIMPRD1 = (Uint16)(period & 0xffff);
	CSL_TIM_0_REGS->TIMPRD2 = (Uint16)(period >> 16);
	// prescale 0000=2
	CSL_TIM_0_REGS->TCR = 0x8003;

	/* Enabling Interrupt */
//	CSL_CPU_REGS->IER0 |= 0x0010;

    CSL_SYSCTRL_REGS->TIAFR = 0x01;
}

