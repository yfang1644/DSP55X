#ifndef		_SCHEDULE_H_
#define		_SCHEDULE_H_

#define	MAX_SCHEDULE	256

typedef struct _timeStruct {
	Uint8  weekday;
	Uint8  year, month, day;
	Uint16 hour, min, sec;		// each holds 2 timers
								// start(LSB) and stop(MSB)
								// ie. 1204 means start at 04:xx:xx,
								//                 stop at 12:xx:xx
	Int32  raw_seconds;
} timeStruct;

void alarmProcessing(void);
void SCHEDULE_init(void);

#endif

