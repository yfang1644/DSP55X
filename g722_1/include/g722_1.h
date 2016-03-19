#ifndef	_G722_1_H
#define	_G722_1_H

#include "tistdtypes.h"
#include "fat.h"

#define	AUDIOPROCESSING	1
/***************************************************************************/
/* Function definitions                                                    */
/***************************************************************************/

void aic3204_init(int rate);
void EZDSP5535_I2S_init(void);
void TIMER_init(void);
void INTR_init(void);
void Audio_init(Uint16 bandwidth);
void PLL_init(Uint32 clock);
void audioProcessing(HANDLE FHandle);

void DSP_word2byte(void *dst, void *src, Uint16 cnt);
void DSP_byte2word(void *dst, void *src, Uint16 cnt);

#endif
