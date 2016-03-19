#ifndef		_AUDIO_H_
#define		_AUDIO_H_

void aic3204_init(unsigned char rate, char adcVol);
void aic3204_set_mic_gain(unsigned char input_gain);
void aic3204_set_spk_gain(unsigned char output_gain);
void aic3204_set_frequency(unsigned char freq);

void EZDSP5535_I2S_init(void);
int audioProcessing(void);
void Audio_init(void);

#endif

