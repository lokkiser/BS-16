/*
 * local_mic.c
 *
 *  Created on: 22 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

//Локальный микрофон

#include "local_mic.h"
#include <string.h>
#include "stm_util.h"
#include "TAS5720.h"

static bool local_mic_is_start = false;

struct adc_mic_snd local_mic_adc;
//mic_adc.index = 0;
//uint16_t [200] = {};
uint32_t d_BASE_AUDIO_FREQUENCY = 48000;
uint32_t d_BASE_FREQUENCY = 165888000;

bool local_mic_is_started(void)
{
	return local_mic_is_start;
}

void local_mic_stop(void)
{
	HAL_ADC_Stop_DMA(LOCAL_MIC_ADC_HANDLE);
	//HAL_TIM_Base_Stop(LOCAL_MIC_ADC_TIM_HANDLE);
	//HAL_TIM_Base_Stop(LOCAL_MIC_DAC_TIM_HANDLE);
	local_mic_is_start = false;
	ulog("local_mic: stop dma"CRLF);
}



bool local_mic_start(void)
{
	HAL_StatusTypeDef status;

	if (local_mic_is_start) 
	{
		local_mic_stop();
	}
	set_tim_frequency2_and_start(LOCAL_MIC_ADC_TIM_HANDLE, d_BASE_AUDIO_FREQUENCY, d_BASE_FREQUENCY);
	HAL_TIM_Base_Start(LOCAL_MIC_ADC_TIM_HANDLE);
	status = HAL_ADC_Start_IT(LOCAL_MIC_ADC_HANDLE);
	if (status != HAL_OK) 
	{
		local_mic_stop();
		ulog_fmt("local_mic: error invalid start dma ADC status = %d"CRLF, status);
		return false;
	}
	return true;
}


