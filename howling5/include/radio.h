//
// FILENAME : radio.h
// This file defines global constants
//

#ifndef	_RADIO_H
#define	_RADIO_H

#include "tistdtypes.h"

#define	FS			32000		// sampling rate in Hz
#define	BUFFER		64

#define	AUDIOPROCESSING	1

typedef	short s16;
typedef	unsigned short u16;
typedef	long s32;
typedef unsigned long u32;


void DSP_zero(void *buf, Int16 cnt);
void DSP_memcpy(void *dst, const void *src, Int16 cnt);

void INTR_init(void);
void PLL_init(Uint32 clock);

void aic3204_init(Uint8 freq, Uint8 gain);

void DMA_audio_init(Uint16 framesize);
void Audio_init(Uint16);
void audioProcessing(int);

#endif	// _RADIO_H
// End of radio.h

