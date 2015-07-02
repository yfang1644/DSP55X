#include "ezdsp5535_sar.h"
#include "ezdsp5535_lcd.h"
#include "radio.h"

static char cfreq[EQBAND][8] = {"500Hz", "1kHz", "2kHz", "4kHz", "8kHz"};
char *freqMessage = "Frequency: ";
char *levelMessage = "Level : ";
Uint16 currentBandPtr = 0;
Int16  bandInc = 1;
Int16  level[EQBAND] = {0, 0, 0, 0, 0};	//-2=-6dB, -1=-3dB,  1=3dB, 2=6dB, ...
Int16  levelInc = 1;

Int16 gKey = NoKey;

void printband() {
	char msg[32];
	int i, j;

	for(i = 0;;i++) {
		if(freqMessage[i] == 0)
			break;
		msg[i] = freqMessage[i];
	}
	for(j = 0;;i++) {
		msg[i] = cfreq[currentBandPtr][j++];
		if(msg[i] == 0)
			break;
	}
	LCD_print(msg, 0);
}

void printlevel() {
	char msg[32];
	int i;
	int l = level[currentBandPtr];

	for(i = 0;;i++) {
		if(levelMessage[i] == 0)
			break;
		msg[i] = levelMessage[i];
	}
	if(l < 0) {
		msg[i++] = '-';
		l = -l;
	}
	l = l * 3;
	if(l >= 20){
		msg[i++] = '2';
		l -= 20;
	} else if(l >= 10) {
		msg[i++] = '1';
		l -= 10;
	}
	msg[i++] = '0' + l;
	msg[i] = '\0';
	LCD_print(msg, 1);
}

void uiProcessing(void)
{
	if(gKey == NoKey)
		return;

	if(gKey == SW1) {
		if(currentBandPtr == 0)
			bandInc = 1;
		if(currentBandPtr == EQBAND - 1)
			bandInc = -1;
		currentBandPtr += bandInc;
		printband();
		printlevel();
	}
	if(gKey == SW2) {
		if(level[currentBandPtr] == -7)
			levelInc = 1;
		if(level[currentBandPtr] == 7)
			levelInc = -1;
		level[currentBandPtr] += levelInc;
		printlevel();
		equalizer(level[currentBandPtr], currentBandPtr);
	}
	gKey = NoKey;
}

