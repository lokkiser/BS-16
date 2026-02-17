/*
 * mp3read.h
 *
 *  Created on: 6 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef mp3read_UTIL_H_
#define mp3read_UTIL_H_



#include "main.h"
#include "lwrb_util.h"
#include "dacplay_util.h"


void mp3play_init(uint8_t * buf, uint16_t buf_size);

void mp3play_task(void);

bool mp3play(const char * file_name, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim);

void mp3play_stop(void);

//!!!need
void mp3play_on_stop(void);

#ifdef MP3PLAY_ON_PCM_WRITE
void mp3play_on_pcm_write(int16_t *pcm16, int pcm16_len);
#endif

#ifdef MP3PLAY_ON_DAC_WRITE
void mp3play_on_dac_write(uint16_t *dac_data, int dac_data_len);
#endif

#endif /* mp3read_UTIL_H_ */
