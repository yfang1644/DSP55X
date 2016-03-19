#include "ezdsp5535_lcd.h"
#include "ezdsp5535_uart.h"
#include "rtcM41Txx_i2c.h"
#include "g722record.h"
#include "ff.h"
#include "fileops.h"
#include "schedule.h"

extern Uint16 taskList;		// bit3 -- schedule processing

timeStruct table[MAX_SCHEDULE];
static Uint16 totalSchedule = 0;
static Int16 curSchedule = -1;
extern recordStruct recordFile;
extern FIL file;

Uint16 asc2bcd(Uint8 *p)
{
	Uint16 i;

	i = (*p++) - '0';
	i = (i << 4) | (*p - '0');
	return i;
}

Uint16 bcd2int(Uint8 bcd)
{
	return ((bcd >> 4) & 0xf) * 10 + (bcd & 0xf);
}

extern Uint8 mix_lr[];		// need extra buffer, since stack limited
Int16 schedule_read(void)
{
	Uint16	i, j;
	Uint8	*tempbuf = mix_lr;
	Uint16	itemcount = 0;
	Uint16	pZero, nbyte, res;
	FIL timetable;

	for(j = 0; j < 256; j++)	tempbuf[j] = 0;

	res = f_open(&timetable, "/SCHEDULE.TXT", FA_READ);
	if (res)
		return 0;

	pZero = 256;
	do {
		nbyte = f_read(&timetable, &tempbuf[pZero], 512);
		i = 0;
		do {
			if((tempbuf[i] >= '0') && (tempbuf[i] <= '6')) {
				table[itemcount].weekday = tempbuf[i] - '0';
				table[itemcount].hour    = asc2bcd(&tempbuf[i +  2]);
				table[itemcount].min     = asc2bcd(&tempbuf[i +  5]);
				table[itemcount].sec     = asc2bcd(&tempbuf[i +  8]);
				table[itemcount].hour   |= asc2bcd(&tempbuf[i + 11]) << 8;
				table[itemcount].min    |= asc2bcd(&tempbuf[i + 14]) << 8;
				table[itemcount].sec    |= asc2bcd(&tempbuf[i + 17]) << 8;
				if(++itemcount >= MAX_SCHEDULE)
					return MAX_SCHEDULE;		// overflow

				i += 3*6;
			}

			if(tempbuf[++i] == '\n') {
				if((i > 512) && (nbyte == 512)) {
					for(j = i; j < pZero + nbyte; j++)
						tempbuf[j - i] = tempbuf[j];
					pZero = pZero + nbyte - i;
					break;
				}
			}
		} while(i < pZero + nbyte);
	} while(nbyte == 512);

	f_close(&timetable);
	return itemcount;		// all table loaded
}

/* Zeller's fomular (Cristian Zeller)
W = [C/4] - 2C + y + [y/4] + [13 * (M+1) / 5] + d - 1
	C=Century
	y=last 2 digits of the year
	M=month (Jan. & Feb. 13 and 14, and y=y-1)
*/

Uint32 checkToday(timeStruct *now)	// return weekday of a date
{
    Uint16 mon[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
	Uint16 year, month, day;
	Uint16 hour, min, sec;
	Uint16 w;
	Int8 bcd[32];
	Uint32 temp, seconds0;

	rtc_get_time(bcd, 8);
	year = bcd2int(bcd[7]);
	month = bcd2int(bcd[6]);
	day = bcd2int(bcd[5]);

    w = year * 365 + mon[month - 1] + (day - 1);
    w += (year >> 2);
    if(year & 3)    w++;		//2000 is a leap year
    else
        if(month > 2)   w++;

	seconds0 = w * 24 * 3600;	// today, 00:00:00, since 2000-01-01
	now->weekday = (w + 6) % 7;		// 2000-01-01 is Saturday

	now->year = year;
	now->month = month;
	now->day = day;
/* calculate seconds since 2000.1.1, 00:00:00 */
	now->hour = bcd[3];
	now->min  = bcd[2];
	now->sec  = bcd[1];

	hour = bcd2int(bcd[3]);
	min  = bcd2int(bcd[2]);
	sec  = bcd2int(bcd[1]);

	temp = seconds0 + hour * 3600 + min * 60 + sec;
	now->raw_seconds = temp;

	return seconds0;
}

Uint32 sumSeconds(timeStruct *p)
{
	Uint32 temp;

	temp = (Uint32)(bcd2int(p->hour)) * 3600 +
			(Uint32)(bcd2int(p->min)) * 60 +
			(Uint32)(bcd2int(p->sec));

	return temp;
}

void adjustDate(timeStruct *p, Uint8 weekday)
{
    Uint8 mon[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	Int8 d = p->weekday - weekday;

	if(d < 0) d += 7;		// till next week
	p->weekday += d;
	p->day += d;

	if(p->day > mon[p->month - 1]) {
		p->day -= mon[p->month - 1];
		p->month++;
		if(p->month > 12) {
			p->month = 1;
			p->year++;
		}
	}
}

void convertSeconds(timeStruct tb[], int item)
{
	int i, j;
	timeStruct today;
	Uint32 sec;

	sec = checkToday(&today);

	for(i = 0; i < item; i++) {			// convert seconds int raw_seconds
		tb[i].year  = today.year;
		tb[i].month = today.month;
		tb[i].day   = today.day;
		adjustDate(&tb[i], today.weekday);

		tb[i].raw_seconds = sec + sumSeconds(&tb[i]) +
							(Uint32)(tb[i].day - today.day) * 3600 * 24;
	}

	sec = today.raw_seconds;
	curSchedule = -1;
	for(i = 0; i < item; i++) {		// sort table
		for(j = i + 1; j < item; j++) {
			if(tb[i].raw_seconds > tb[j].raw_seconds) {
				today = tb[i];
				tb[i] = tb[j];
				tb[j] = today;
			}
		}
		if((curSchedule == -1) && (tb[i].raw_seconds > sec)) {
			curSchedule = i;
		}
	}
}

void SCHEDULE_init(void)
{
	char *prt = "SCHEDULE   0";
	Uint8 n, tmp;

	totalSchedule = schedule_read();
	if (totalSchedule == 0) {
		LCD_print("SCHEDULE 0", 1);
		return;
	}

	tmp = totalSchedule;
	for(n = 0; tmp >= 100; n++)	tmp -= 100;
	prt[9] = n + '0';
	for(n = 0; tmp >= 10; n++)	tmp -= 10;
	prt[10] = n + '0';
	prt[11] = tmp + '0';
	LCD_print(prt, 0);

	convertSeconds(table, totalSchedule);
	rtc_set_alarm(table[curSchedule], 0);		// set first alarm
}

void alarmProcessing(void)
{
	if((taskList & ALARMPROCESSING) == 0)   return;

	if((recordFile.function & RECORD) == 0) {
//		start_record();
		rtc_set_alarm(table[curSchedule], 8);	// set stop alarm
	} else {
//		f_close(&file);
		recordFile.function = DIRECT_IO;

		curSchedule++;
		if(curSchedule >= totalSchedule)
			curSchedule = 0;

		rtc_set_alarm(table[curSchedule], 0);
	}

	taskList &= ~ALARMPROCESSING;
}

