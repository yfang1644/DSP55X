#include "tistdtypes.h"

typedef enum _CHANNEL {
	LEFT = 0,
	RIGHT = 1,
	STEREO = 2
} CHANNEL;

#define AUDIOPROCESSING     1
#define FILEPROCESSING      2
#define SPIPROCESSING       4
#define GPIOPROCESSING      8

void DSP_memcpy(void *dst, void *src, Int16 cnt);

/*****************************************************
 * prototype in main
 ****************************************************/
void aic3204_init(Uint8 rate, Int8 micgain);
void EZDSP5535_I2S_init(void);
void TIMER_init(Uint32 clock);
void INTR_init(void);
void EZDSP5535_PLL_init(Uint32 clock);
void DMA_audio_init(Uint16 size);

void audioEncoding(int chn);
void audioDecoding(void);
void Audio_init(void);

