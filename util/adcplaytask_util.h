/*
 * adcplay.h
 *
 *  Created on: 21 февр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_adcplaytask_UTIL_H_
#define CORE_INC_adcplaytask_UTIL_H_

#include "main.h"
#include <stdint.h>
#include "def_util.h"

#ifndef	adcplaytask_MAX_BUF_SIZE
#define adcplaytask_MAX_BUF_SIZE	2560
#endif

typedef struct _adcplaytask_data 
{
	uint8_t id;
	uint8_t index;
	uint16_t dma_buf[2][adcplaytask_MAX_BUF_SIZE];
	bool dma_start;
	ADC_HandleTypeDef* hadc;
	TIM_HandleTypeDef* htim;
} adcplaytask_data_t;

void adcplaytask_init(adcplaytask_data_t* adcplaytask_data, uint8_t id, ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* htim);
void adcplaytask_HalfCpltCallback(adcplaytask_data_t* adcplaytask_data);
void adcplaytask_FullCpltCallback(adcplaytask_data_t* adcplaytask_data);
void adcplaytask_start_dma(adcplaytask_data_t* adcplaytask_data, uint32_t frequency);
void adcplaytask_stop_dma(adcplaytask_data_t* adcplaytask_data);
void adcplaytask_task(adcplaytask_data_t* adcplaytask_data);
bool adcplaytask_is_dma_started(adcplaytask_data_t* adcplaytask_data);

//!!!! need
void adcplaytask_data_ready(adcplaytask_data_t* adcplaytask_data, uint16_t* adc_data, uint16_t adc_data_len);




#endif /* CORE_INC_adcplaytask_UTIL_H_ */
