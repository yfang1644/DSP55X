//////////////////////////////////////////////////////////////////////////////
// * File name: rtcM41TXX_i2c.h
//////////////////////////////////////////////////////////////////////////////
#include"ezdsp5535_i2c.h"
#include "schedule.h"

void RTC_reset(void);
void rtc_get_time(Int8 *str, int len);
void rtc_set_time(Int8 *str, int len);
void rtc_set_alarm(timeStruct time, int start);

