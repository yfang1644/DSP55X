//
// FILENAME : radio.h
// This file defines global constants
//

#ifndef	_RADIO_H
#define	_RADIO_H

#include "tistdtypes.h"

#define	FS			48000		// sampling rate in Hz
#define	FRAMESIZE	960

#define	AUDIOPROCESSING	1

void INTR_init(void);
void PLL_init(Uint32 clock);

void aic3204_init(Uint8 freq, Uint8 gain);

void DMA_audio_init(Uint16 framesize);
void Audio_init(Uint16);
void audioProcessing(Uint16 func, Uint16 channel);
void DSP_memcpy(void *dst, void *src, Uint16 cnt);

#endif	// _RADIO_H
// End of radio.h

