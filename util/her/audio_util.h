/*
 * audio_util.h
 *
 *  Created on: 8 июн. 2020 г.
 *      Author: zugan
 */

#ifndef AUDIO_UTIL_H_
#define AUDIO_UTIL_H_

#include <stdint.h>
#include "fatfs_util.h"


#ifndef ADC2DAC_VOLUME_MIDDLE
#define ADC2DAC_VOLUME_MIDDLE(adc, volume) 	2047 + (adc - 2047) * volume
#endif



#define AUDUI_BUFFER_WRITE(bytes, bytes_cnt)	lwrb_write(&audio_buffer_lwrb, bytes, bytes_cnt)
#define AUDUI_BUFFER_FULL						lwrb_get_full(&audio_buffer_lwrb)
#define AUDUI_BUFFER_READ(bytes, bytes_cnt)		lwrb_read(&audio_buffer_lwrb, bytes, bytes_cnt)


extern FIL audio_file;


#ifdef __cplusplus
extern "C" {
#endif

//void pcmdata2dacdata(int16_t * pcm_data, uint16_t * dac_data, uint16_t data_len, uint16_t volume);

//void dacdata2pcmdata(uint16_t * dac_data, int16_t * pcm_data, uint16_t data_len, uint16_t volume);



#ifdef __cplusplus
}
#endif


#endif /* LIB_MYAUDIO_H_ */
