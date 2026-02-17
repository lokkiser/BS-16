/*
 * wavplay_util.h
 *
 *  Created on: 22 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_WAVPLAY_UTIL_H_
#define UTIL_WAVPLAY_UTIL_H_

#include "main.h"
#include "dacplay_util.h"


bool wavplay(const char * wav_file_name, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim);
void wavplay_task(void);
void wavplay_stop(void);
//need
void wavplay_on_stop(void);


#endif /* UTIL_WAVPLAY_UTIL_H_ */
