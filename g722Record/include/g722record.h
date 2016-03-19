#ifndef	_G722RECORD_H
#define	_G722RECORD_H

typedef enum _CHANNEL {
    LEFT = 0,
    RIGHT = 1,
    STEREO = 2
} CHANNEL;

#define AUDIOPROCESSING     1
#define FILEPROCESSING      2
#define UARTPROCESSING      4
#define ALARMPROCESSING     8
#define USBPROCESSING       16

void DSP_watermark(void *dst, const void *src, Int16 cnt);

/*****************************************************
 * prototypes
 ****************************************************/

void TIMER_init(void);
void INTR_init(void);
void PLL_init(Uint32 clock);
void powerManager(Uint16 on);

Int16 BoardUSB_init(void);
void usbProcessing(void);

#define	WATERMARK_CODE	0x0841

void aic3204_init(unsigned char rate, char adcVol);
void aic3204_set_mic_gain(unsigned char input_gain);
void aic3204_set_spk_gain(unsigned char output_gain);
void aic3204_set_frequency(unsigned char freq);

void EZDSP5535_I2S_init(void);
void audioProcessing(void);
void Audio_init(unsigned short bandwidth);

void DSP_word2byte(void *dst, void *src, Uint16 cnt);
void DSP_byte2word(void *dst, void *src, Uint16 cnt);

#endif	//_G722RECORD_H

