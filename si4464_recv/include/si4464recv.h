#ifndef	_SI4464RECV_H
#define	_SI4464RECV_H

typedef enum _CHANNEL {
    LEFT = 0,
    RIGHT = 1,
    STEREO = 2
} CHANNEL;

#define AUDIOPROCESSING     1
#define FILEPROCESSING      2
#define SPIPROCESSING       4
#define GPIOPROCESSING      8

/*****************************************************
 * prototypes
 ****************************************************/

void aic3204_init(Uint8 rate, Int8 micGain);
void EZDSP5535_I2S_init(void);
void TIMER_init(Uint32 period);
void INTR_init(void);
void EZDSP5535_PLL_init(Uint32 clock);
void Audio_init(Uint16 bandwidth);

void audioEncoding(int chn);
void audioDecoding(void);
void fileProcessing(void);

#endif		// _SI4464RECV_H
