//
// FILENAME : radio.h
// This file defines global constants
//

#ifndef	_RADIO_H
#define	_RADIO_H

#include "tistdtypes.h"

#define	FS			48000		// sampling rate in Hz
#define	FRAMESIZE	32

#define	FS_R		(22370L)	// 2*PI/FS, scaled to 1/FS, Q1.15*32768

#define	AUDIOPROCESSING		1
#define	MUSICPROCESSING		2

typedef struct _instrument {
	int Ao;
	int fm;
} instrument;

typedef struct _note {
	Uint16 stage:    4;
	Uint16 half:     1;
	Uint16 tone :    3;
	Uint16 duration: 8;
} note;

void DSP_zero(void *buf, Int16 cnt);

void INTR_init(void);
void PLL_init(Uint32 clock);
void TIMER_init(Uint32 clock);

void aic3204_init(Uint8 freq, Uint8 gain);

void DMA_audio_init(Uint16 fra0mesize);
void Audio_init(Uint16);
void audioProcessing(void);
Int16  musicProcessing(Int16 start);
void changeInstrument(Uint16);

Int16 sine(Int16 arc);
#endif	// _RADIO_H
// End of radio.h

