#include "ezdsp5535.h"

//#define M41TXX_ADDR		0x68				// M41TXX I2C address
#ifndef	M41TXX_ADDR
void RTC_reset(void)
{
	CSL_RTC_REGS->RTCOSC = 0x8008;
    /* enable rtc clock out  */
	CSL_RTC_REGS->RTCPMGT = 0x0001;
}

void rtc_set_time (unsigned char *pRtcTime, int len)
{
	volatile Uint16 timeOut;

	timeOut = 0x400;
    /* before writing the time to RTC Registers clear time update bit */
    CSL_RTC_REGS->RTCUPDATE &= ~0x8000;

	CSL_RTC_REGS->RTCMIL  = pRtcTime[0];
	CSL_RTC_REGS->RTCSEC  = pRtcTime[1];
	CSL_RTC_REGS->RTCMIN  = pRtcTime[2];
	CSL_RTC_REGS->RTCHOUR = pRtcTime[3];

	CSL_RTC_REGS->RTCDAY   = pRtcTime[5];
	CSL_RTC_REGS->RTCMONTH = pRtcTime[6];
	CSL_RTC_REGS->RTCYEAR  = pRtcTime[7];

	/* Time updates enabled */
    CSL_RTC_REGS->RTCUPDATE |= 0x8000;
    while((CSL_RTC_REGS->RTCUPDATE & 0x8000) && (--timeOut != 0))
		;
}

void rtc_get_time (unsigned char *pRtcTime, int len)
{
	pRtcTime[0] = CSL_RTC_REGS->RTCMIL;
	pRtcTime[1]  = CSL_RTC_REGS->RTCSEC;
	pRtcTime[2]  = CSL_RTC_REGS->RTCMIN;
	pRtcTime[3]  = CSL_RTC_REGS->RTCHOUR;

	pRtcTime[5]  = CSL_RTC_REGS->RTCDAY;
	pRtcTime[6]  = CSL_RTC_REGS->RTCMONTH;
	pRtcTime[7]  = CSL_RTC_REGS->RTCYEAR;
}

#endif		// M41TXX_ADDR

