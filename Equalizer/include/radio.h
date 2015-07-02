//
// FILENAME : radio.h
// This file defines global constants
//

#ifndef	_RADIO_H
#define	_RADIO_H

#include "tistdtypes.h"

#define	FS			48000		// sampling rate in Hz
#define	FRAMESIZE	32
#define	EQBAND		5

#define	AUDIOPROCESSING	1
#define	UIPROCESSING	2

typedef struct _biquad {
	Int16 a2;       /* a2 < 1 */
	Int16 a1;       /* -2 < a1 < -1 */
	Int16 b2;       /* Max. -5.3 */
	Int16 b1;       /* b1 == a1 */
	Int16 b0;       /* Max.  6.7 */
} biquad;

void DSP_memcpy(void *dst, const void *src, Int16 cnt);

void INTR_init(void);
void PLL_init(Uint32 clock);
void TIMER_init(Uint32 period);

void aic3204_init(Uint8 freq, Uint8 gain);

void DMA_audio_init(Uint16 framesize);
void Audio_init(Uint16);
void audioProcessing(void);

void equalizer(Int16 level, int band);
void InitFilter(void);
void clearBuf(void);
#endif	// _RADIO_H
// End of radio.h

