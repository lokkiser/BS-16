/*
 * memplay.h
 *
 *  Created on: 8 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_MEMPLAY_H_
#define CORE_INC_MEMPLAY_H_

#include "main.h"
#include "dacplay_util.h"

void memplay_init(uint8_t * buf, uint16_t buf_size);

void memplay_stop(void);

void memplay_task(void);

void memplay(const uint8_t * mem, uint32_t mem_size, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim);

///need
void memplay_on_stop(void);

#endif /* CORE_INC_MEMPLAY_H_ */
