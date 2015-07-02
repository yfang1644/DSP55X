//
// FILENAME : radio.h
// This file defines global constants
//

#ifndef	_RADIO_H
#define	_RADIO_H

#include "tistdtypes.h"

#define	FS			48000		// sampling rate in Hz
#define	FRAMESIZE	960

#define	AUDIOPROCESSING		1
#define	FILEPROCESSING		2
#define	UARTPROCESSING		4

typedef struct {long re, im;} Complex;

typedef struct {int mant; int index;} fraction;

void INTR_init(void);
void PLL_init(Uint32 clock);
void TIMER_init(Uint32 period);

void Audio_init(void);
void aic3204_init(Uint8 freq, Uint8 gain);
void audioProcessing(void);

#endif	// _RADIO_H
// End of radio.h

