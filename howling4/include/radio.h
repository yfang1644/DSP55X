//
// FILENAME : radio.h
// This file defines global constants
//

#ifndef	_RADIO_H
#define	_RADIO_H

#include "tistdtypes.h"

#define	FS			32000		// sampling rate in Hz
#define	M			10			// power of 2
#define	FFT_L		(1<<M)		// length of FFT

#define AUDIOPROCESSING     1
#define FILEPROCESSING      2
#define UARTPROCESSING      4

typedef	short s16;
typedef	unsigned short u16;
typedef	long s32;
typedef unsigned long u32;

typedef struct {long re, im;} Complex;

typedef struct {int mant; int index;} fraction;


void DSP_zero(void *buf, Int16 cnt);
void DSP_memcpy(void *dst, const void *src, Int16 cnt);


void TIMER_init(void);
void INTR_init(void);
void PLL_init(Uint32 clock);

void aic3204_init(int freq, int gain);

void DMA_audio_init(Uint16 framesize);
void Audio_init(Uint16);
void audioProcessing(int func);

#endif	// _RADIO_H
// End of radio.h

