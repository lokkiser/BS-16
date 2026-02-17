/*
 * adcplay.c
 *
 *  Created on: 21 февр. 2024 г.
 *      Author: Rafael Boltachev
 */
#include "adcplaytask_util.h"
#include "stm_util.h"


void adcplaytask_HalfCpltCallback(adcplaytask_data_t* adcplaytask_data)
{
	adcplaytask_data->index = 0;
}

void adcplaytask_FullCpltCallback(adcplaytask_data_t* adcplaytask_data)
{
	adcplaytask_data->index = 1;
}

void adcplaytask_stop_dma(adcplaytask_data_t* adcplaytask_data)
{
	HAL_ADC_Stop_DMA(adcplaytask_data->hadc);
	//if (adcplaytask_data->htim) HAL_TIM_Base_Stop(adcplaytask_data->htim);
	adcplaytask_data->index = 0xFF;
	adcplaytask_data->dma_start = false;
	ulog_fmt("adcplay: stop dma (id:%d)"CRLF, adcplaytask_data->id);
}

bool adcplaytask_is_dma_started(adcplaytask_data_t* adcplaytask_data)
{
	return adcplaytask_data->dma_start;
}

void adcplaytask_start_dma(adcplaytask_data_t* adcplaytask_data, uint32_t frequency)
{
	if (adcplaytask_data->dma_start) adcplaytask_stop_dma(adcplaytask_data);

	if (!frequency) return;

	if (adcplaytask_data->htim) 
	{
		set_tim_frequency2(adcplaytask_data->htim, frequency, BASE_TIM_FREQUENSY);
		HAL_TIM_Base_Start(adcplaytask_data->htim);
	}
	HAL_ADC_Start_DMA(adcplaytask_data->hadc, (uint32_t*)adcplaytask_data->dma_buf, adcplaytask_MAX_BUF_SIZE * 2);
	adcplaytask_data->dma_start = true;
	ulog_fmt("adcplay: start dma frequency:%lu(id:%d)"CRLF, frequency, adcplaytask_data->id);
}

void adcplaytask_task(adcplaytask_data_t* adcplaytask_data)
{
	if (adcplaytask_data->index < 2) 
	{
		adcplaytask_data_ready(adcplaytask_data, adcplaytask_data->dma_buf[adcplaytask_data->index], adcplaytask_MAX_BUF_SIZE);
		adcplaytask_data->index = 0xFF;
	}
}

void adcplaytask_init(adcplaytask_data_t* adcplaytask_data, uint8_t id, ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* htim)
{
	adcplaytask_data->id = id;
	adcplaytask_data->index = 0xFF;
	adcplaytask_data->dma_start = false;
	adcplaytask_data->hadc = hadc;
	adcplaytask_data->htim = htim;
}
