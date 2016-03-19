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

void cfft32_NOSCALE(Complex *x, u16 nx);
void cbrev32(Complex *x, Complex *y, u16 nx);
void DSP_restore(Complex *x, short *dst, int nx, int scale);

void DSP_zero(Int16 *buf, Int16 cnt);
void DSP_memcpy(Int16 *dst, const Int16 *src, Int16 cnt);


void TIMER_init(void);
void INTR_init(void);
void PLL_init(Uint32 clock);

void aic3204_init(int freq, int gain);

void DMA_audio_init(Uint16 framesize);
void Audio_init(Uint16);
void audioProcessing(int func);

#endif	// _RADIO_H
// End of radio.h

