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

static bool local_mic_is_start = false;
static uint16_t local_mic_dma_adc_buf[2][SOUND_DMA_BUFFER_SIZE];
static uint16_t local_mic_dma_dac_buf[2][SOUND_DMA_BUFFER_SIZE];
static uint8_t local_mic_dma_dac_index = 0xFF;



void local_mic_dac_HalfCpltCallback(void)
{
	local_mic_dma_dac_index = 0;

	//fulog("d0"CRLF);
}

void local_mic_dac_FullCpltCallback(void)
{
	local_mic_dma_dac_index = 1;

	//fulog("d1"CRLF);
}

void local_mic_adc_HalfCpltCallback(void)
{

	if (local_mic_dma_dac_index < 2) 
	{
		memcpy(local_mic_dma_dac_buf[local_mic_dma_dac_index], local_mic_dma_adc_buf[0], SOUND_DMA_BUFFER_SIZE * 2);
		//fulog_fmt("a0->d%d"CRLF, local_mic_dma_dac_index);
		local_mic_dma_dac_index = 0xFF;
	}
	//fulog("a0"CRLF);
}

void local_mic_adc_FullCpltCallback(void)
{
	if (local_mic_dma_dac_index < 2) 
	{
		memcpy(local_mic_dma_dac_buf[local_mic_dma_dac_index], local_mic_dma_adc_buf[1], SOUND_DMA_BUFFER_SIZE * 2);
		//fulog_fmt("a1->d%d"CRLF, local_mic_dma_dac_index);
		local_mic_dma_dac_index = 0xFF;
	}

	//fulog("a1"CRLF);
}

bool local_mic_is_started(void)
{
	return local_mic_is_start;
}

void local_mic_stop(void)
{
	HAL_ADC_Stop_DMA(LOCAL_MIC_ADC_HANDLE);
	//HAL_TIM_Base_Stop(LOCAL_MIC_ADC_TIM_HANDLE);
	HAL_DAC_Stop_DMA(LOCAL_MIC_DAC_HANDLE, LOCAL_MIC_DAC_CHANNEL);
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

	//set_tim_frequency2(LOCAL_MIC_ADC_TIM_HANDLE, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);
	//HAL_TIM_Base_Start(LOCAL_MIC_ADC_TIM_HANDLE);
	status = HAL_ADC_Start_DMA(LOCAL_MIC_ADC_HANDLE, (uint32_t *)local_mic_dma_adc_buf, SOUND_DMA_BUFFER_SIZE * 2);
	if (status != HAL_OK) 
	{
		local_mic_stop();
		ulog_fmt("local_mic: error invalid start dma adc status = %d"CRLF, status);
		return false;
	}

	//set_tim_frequency2(LOCAL_MIC_DAC_TIM_HANDLE, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);
	//HAL_TIM_Base_Start(LOCAL_MIC_DAC_TIM_HANDLE);
	status = HAL_DAC_Start_DMA(LOCAL_MIC_DAC_HANDLE, LOCAL_MIC_DAC_CHANNEL, (uint32_t * )local_mic_dma_dac_buf, SOUND_DMA_BUFFER_SIZE * 2, DAC_ALIGN_12B_R);
	if (status != HAL_OK) 
	{
		local_mic_stop();
		ulog_fmt("local_mic: error invalid start dma dac status = %d"CRLF, status);
		return false;
	}

	local_mic_is_start = true;
	ulog("local_mic: start dma"CRLF);
	return true;
}


