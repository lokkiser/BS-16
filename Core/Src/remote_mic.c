/*
 * remote_mic.c
 *
 *  Created on: Sep 9, 2026
 *      Author: bimpk
 */

#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "remote_mic.h"
#include "TAS5720.h"
#include "console.h"
#include "str_util.h"
#include <string.h>

struct adc_mic_snd mic_adc;
//mic_adc.index = 0;
//uint16_t [200] = {};
static uint16_t remote_mic_dma_adc_buf[2][SOUND_DMA_BUFFER_SIZE];
static uint16_t remote_mic_dma_dac_buf[2][SOUND_DMA_BUFFER_SIZE];
static uint8_t  remote_mic_dma_dac_index = 0xFF;
static bool remote_mic_is_start = false;

void remote_mic_dac_HalfCpltCallback(void)
{
	remote_mic_dma_dac_index = 0;

	//fulog("d0"CRLF);
}

void remote_mic_dac_FullCpltCallback(void)
{
	remote_mic_dma_dac_index = 1;

	//fulog("d1"CRLF);
}

void remote_mic_adc_HalfCpltCallback(void)
{

	if (remote_mic_dma_dac_index < 2)
	{
		memcpy(remote_mic_dma_dac_buf[remote_mic_dma_dac_index], remote_mic_dma_adc_buf[0], SOUND_DMA_BUFFER_SIZE * 2);
		//fulog_fmt("a0->d%d"CRLF, remote_mic_dma_dac_index);
		remote_mic_dma_dac_index = 0xFF;
	}
	//fulog("a0"CRLF);
}

void remote_mic_adc_FullCpltCallback(void)
{
	if (remote_mic_dma_dac_index < 2)
	{
		memcpy(remote_mic_dma_dac_buf[remote_mic_dma_dac_index], remote_mic_dma_adc_buf[1], SOUND_DMA_BUFFER_SIZE * 2);
		//fulog_fmt("a1->d%d"CRLF, remote_mic_dma_dac_index);
		remote_mic_dma_dac_index = 0xFF;
	}

	//fulog("a1"CRLF);
}

bool remote_mic_is_started(void)
{
	return remote_mic_is_start;
}

void remote_mic_stop(void)
{
	HAL_ADC_Stop_DMA(LOCAL_MIC_ADC_HANDLE);
	//HAL_TIM_Base_Stop(LOCAL_MIC_ADC_TIM_HANDLE);
	HAL_DAC_Stop_DMA(LOCAL_MIC_DAC_HANDLE, LOCAL_MIC_DAC_CHANNEL);
	//HAL_TIM_Base_Stop(LOCAL_MIC_DAC_TIM_HANDLE);
	remote_mic_is_start = false;
	ulog("remote_mic: stop dma"CRLF);
}


bool remote_mic_start(void)
{
	HAL_StatusTypeDef status;

	/*if (remote_mic_is_start)
	{
		remote_mic_stop();
	}
	set_tim_frequency2_and_start(remote_mic_ADC_TIM_HANDLE, d_BASE_AUDIO_FREQUENCY, d_BASE_FREQUENCY);
	// set_tim_frequency2(remote_mic_ADC_TIM_HANDLE, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);*/
	//HAL_TIM_Base_Start(remote_mic_ADC_TIM_HANDLE);
	HAL_ADC_Start_DMA(REMOTE_MIC_ADC_HANDLE, (uint32_t*)remote_mic_dma_adc_buf, SOUND_DMA_BUFFER_SIZE * 2);
	//HAL_DAC_Start_DMA
	/*status = HAL_ADC_Start_IT(remote_mic_ADC_HANDLE);
	if (status != HAL_OK)
	{
		remote_mic_stop();
		ulog_fmt("remote_mic: error invalid start dma ADC status = %d"CRLF, status);
		return false;
	}*/

	// set_tim_frequency2(remote_mic_DAC_TIM_HANDLE, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);
	// HAL_TIM_Base_Start(remote_mic_DAC_TIM_HANDLE);
	//тут через SAI и прерывания
	status = Audio_System_Start();
	if (status != HAL_OK)
	{
		//remote_mic_stop();
		//ulog_fmt("remote_mic: error invalid start dma DAC status = %d"CRLF, status);
		return false;
	}

	remote_mic_is_start = true;
	//ulog("remote_mic: start dma"CRLF);
	return true;
}
