#include "csl_intc.h"

#include "ezdsp5535.h"
#include "g722record.h"
#include "schedule.h"

//#define M41TXX_ADDR		0x68				// M41TXX I2C address
#ifndef	M41TXX_ADDR
void RTC_reset(void)
{
	CSL_RTC_REGS->RTCOSC = 0x8008;
    /* enable rtc clock out  */
	CSL_RTC_REGS->RTCPMGT = 0x0001;
}

void rtc_set_time(Int8 *pRtcTime, int len)
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

void rtc_get_time(Int8 *pRtcTime, int len)
{
	pRtcTime[0]	= CSL_RTC_REGS->RTCMIL;
	pRtcTime[1]	= CSL_RTC_REGS->RTCSEC;
	pRtcTime[2]	= CSL_RTC_REGS->RTCMIN;
	pRtcTime[3]	= CSL_RTC_REGS->RTCHOUR;

	pRtcTime[5]	= CSL_RTC_REGS->RTCDAY;
	pRtcTime[6]	= CSL_RTC_REGS->RTCMONTH;
	pRtcTime[7]	= CSL_RTC_REGS->RTCYEAR;
}

Uint8 hex2bcd(Uint8 hex)
{
	Uint8 bcd;
	Uint16 i = 0;

	while(hex >= 10) {
		hex -= 10;
		i++;
	}

	bcd = (i << 4) | hex;
	return bcd;
}

void rtc_set_alarm(timeStruct time, int pos)	// 0:start, 8:stop
{
	volatile Uint16 timeOut = 0x400;

    /* before writing the time to RTC Registers clear alarm update bit */
	CSL_RTC_REGS->RTCUPDATE &= ~0x4000;

	CSL_RTC_REGS->RTCMILA  = 0;

	CSL_RTC_REGS->RTCSECA  = (time.sec >> pos) & 0x0ff;
	CSL_RTC_REGS->RTCMINA  = (time.min >> pos) & 0x0ff;
	CSL_RTC_REGS->RTCHOURA = (time.hour >> pos) & 0x0ff;

	CSL_RTC_REGS->RTCDAYA   = hex2bcd(time.day);
	CSL_RTC_REGS->RTCMONTHA = hex2bcd(time.month);
	CSL_RTC_REGS->RTCYEARA  = hex2bcd(time.year);

	/* bit15 : alarm
	   bit4-bit0: day, hr, min, sec, ms */

	/* Alarm updates enabled */
    CSL_RTC_REGS->RTCUPDATE |= 0x4000;
    while((CSL_RTC_REGS->RTCUPDATE & 0x4000) && (--timeOut != 0))
		;
	CSL_RTC_REGS->RTCINTREG = 0x8000;		// RTC alarm interrupt enable
	CSL_RTC_REGS->RTCINTEN  = 0x0001;		// RTC interrupt enable
	CSL_RTC_REGS->RTCPMGT &= ~(1 << 3);
	CSL_CPU_REGS->IER1 |= (1 << (RTC_EVENT - 16));
}

extern Uint16 taskList;

void interrupt rtc_isr(void)
{
	CSL_GPIO_REGS->IOOUTDATA1 ^= 0x4000;    // blue LED
	CSL_RTC_REGS->RTCINTFL  = 0x8000;		//
	taskList |= ALARMPROCESSING;
}

#endif		// M41TXX_ADDR

