#include "ezdsp5535.h"
#include "radio.h"

Int16 freqTable[] = {		// 2^8 * 55 * 2^((0:11)/12)
	14080, 14917, 15804, 16744, 17740, 18795,
	19912, 21096, 22351, 23680, 25088, 26580};

instrument preDefined[] = {
	{0    , 0},
	{0x300, 50},
	{0x500, 8},
	{0x2000, 5},
	{0x1000, 6}
};

Uint16 song[] = {
0x2610, 0x3110, 0x3310, 0x3110, 0x3220, 0x3110, 0x2710, 0x3320, 0x3220, 0x2640,
0x3110, 0x3310, 0x3510, 0x3510, 0x3620, 0x3510, 0x3410, 0x3340,
0x3420, 0x3520, 0x3710, 0x3610, 0x3320, 0x3310, 0x3110, 0x2710, 0x2610, 0x3310, 0x3210, 0x3440,
0x3510, 0x3410, 0x3320, 0x3210, 0x3110, 0x3320, 0x3220, 0x2640,
0};

extern Uint16 taskList;

Int16 dPhi = 100;
Int16 dArg = 0x1000;
Uint16 gVolMask = 0xffff;
Uint16 Ao = 0x200;

Int16 musicProcessing(Int16 start)
{
	static Uint16 songPtr = 0;
	note  *curNote;
	Uint16 fp, freq;
	Uint32 Ltemp;

	if(start)	songPtr = 0;

	if((taskList & MUSICPROCESSING) == 0)   return 0;
	taskList &= ~MUSICPROCESSING;

	curNote = (note *)&song[songPtr];
	gVolMask = 0xffff;
	if (*(Uint16 *)curNote == 0) {
		gVolMask = 0;
		return 0;
	}

	switch(curNote->tone) {
	case 0:
		gVolMask = 0; break;
	case 1:
	case 2:
	case 3:
		fp = (curNote->tone - 1) * 2; break;
	case 4:
	case 5:
	case 6:
	case 7:
		fp = (curNote->tone -1) * 2 - 1; break;
	default:
		break;
	}
	if (curNote->half) fp++;
	freq   = freqTable[fp];
	Ltemp  = freq * FS_R;		// FS_R=Q1.15*32768
	Ltemp  = Ltemp >> 15;		// sine_table[32768]

	dArg   = (Int16)(Ltemp >> (8 - curNote->stage));
	TIMER_init((Uint32)curNote->duration << 20);
	songPtr++;

	return 0;
}

void changeInstrument(Uint16 number)
{
	Ao = preDefined[number].Ao;
	dPhi = preDefined[number].fm;
}


