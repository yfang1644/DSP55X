//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_lcd.c
//////////////////////////////////////////////////////////////////////////////

#include"ezdsp5535_i2c.h"

#define M41TXX_ADDR		0x68				// M41TXX I2C address
#undef	M41TXX_ADDR

#ifdef	M41TXX_ADDR
void rtc_get_time(Int8 *str, int len)
{
	str[0] = 0;
	EZDSP5535_I2C_write(M41TXX_ADDR, str, 1);
	EZDSP5535_waitusec(1000);
	EZDSP5535_I2C_read(M41TXX_ADDR, str, len);
}

/* 
 *  void rtc_set_time( )
 *  Set RTC time. string:
 *     addr ms sec min hour weekday date century/month year (9 bytes)
 */
void rtc_set_time(Int8 *str, int len)
{
	EZDSP5535_waitusec(1000);
	EZDSP5535_I2C_write(M41TXX_ADDR, str, len);
}

/* 
 *  void rtc_set_alarm( )
 *  Set RTC alarm. string:
 *     addr ms sec min hour weekday date century/month year (9 bytes)
 */
void rtc_set_alarm(Int8 *str)
{
	Int8 alarm[16];

	alarm[0] = 0x0A;						// once per day, RPT:11000
	alarm[1] = (*str++) | 0x80;				// month | Alarm Enable
	alarm[2] = (*str++) | 0xC0;				// date | RPT4 |RPT5
	alarm[3] = (*str++) | 0x00;				// hour | RPT3
	alarm[4] = (*str++) | 0x00;				// min. | RPT2
	alarm[5] = (*str++) | 0x00;				// sec. | RPT1

	EZDSP5535_waitusec(1000);
	EZDSP5535_I2C_write(M41TXX_ADDR, alarm, 6);
}

#endif		// M41TXX_ADDR

