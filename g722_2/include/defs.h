#include "typedef.h"

/* Max bit rate is 48000 bits/sec. */
#define MAX_BITS_PER_FRAME 960



void DSP_zero(Int16 *buf, Int16 cnt);
void DSP_memcpy(Int16 *dst, Int16 *src, Int16 cnt);
void DSP_add(Int16 *dst, Int16 *src, Int16 cnt);
Int16 DSP_mac(Int16 *ptr1, Int16 *ptr2, Int16 cnt);
void DSP_byte2word(Uint8 *src, Int16 *dst, Int16 cnt);
void DSP_word2byte(Uint8 *dst, Int16 *src, Int16 cnt);

/*****************************************************
 * prototype in main
 ****************************************************/
void aic3204_init(int rate);
void EZDSP5535_I2S_init(void);
void TIMER_init(void);
void INTR_init(void);
void PLL_init(Uint32 clock);
void DMA_audio_init(Uint16 size);


void audioProcessing(void);
void audio_init(void);

