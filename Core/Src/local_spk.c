/*
 * local_spk.c
 *
 *  Created on: 19 янв. 2026 г.
 *      Author: Rafael Boltachev
 */


#include "local_spk.h"
#include <string.h>
#include "stm_util.h"
#include "TAS5720.h"
//что за волшебные числа? 11? 3?
dacplay_data_t spk_dacplay_data = DACPLAY_STATIC_INIT(11, LOCAL_SPK_DAC_HANDLE, LOCAL_SPK_DAC_CHANNEL);
adcplay_data_t spk_adcplay_data = ADCPLAY_STATIC_INIT( 3, LOCAL_SPK_ADC_HANDLE);


uint8_t sound_buf[SOUND_BUF_SIZE];

void memplay_on_stop(void)
{
	ulog("spk: memplay_on_stop"CRLF);
    set_tim_frequency2(&htim15, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);
	HAL_TIM_Base_Start(&htim15);
}


void adcplay_write_data(adcplay_data_t * adcplay_data, uint16_t * adc_data)
{
	if (adcplay_data == &spk_adcplay_data) 
	{
		if (spk_dacplay_data.index < 2) 
		{
			memcpy(spk_dacplay_data.dma_buf[spk_dacplay_data.index], adc_data, ADCPLAY_DMA_BUF_SIZE * 2);
			spk_dacplay_data.index = 0xFF;
		}
	}
}


void local_spk_dac_HalfCpltCallback(void)
{
	dacplay_HalfCpltCallback(&spk_dacplay_data);
	//fulog("d0"CRLF);
}

void local_spk_dac_FullCpltCallback(void)
{
	dacplay_FullCpltCallback(&spk_dacplay_data);
	//fulog("d1"CRLF);
}

void local_spk_adc_HalfCpltCallback(void)
{
	adcplay_HalfCpltCallback(&spk_adcplay_data);
	//fulog("a0"CRLF);
}

void local_spk_adc_FullCpltCallback(void) //тангента
{

	//должен писать в буфер
	//adcplay_FullCpltCallback(&spk_adcplay_data);
	//fulog("a1"CRLF);
}

bool local_spk_is_started(void)
{
	return dacplay_is_dma_started(&spk_dacplay_data) || adcplay_is_dma_started(&spk_adcplay_data);
}

void local_spk_stop(void)
{
	adcplay_stop_dma(&spk_adcplay_data);
	dacplay_stop_dma(&spk_dacplay_data);
}

bool local_spk_start(void)
{
	bool res = 1;

	//dacplay_init(dacplay_data_t * dacplay_data, uint8_t id, DAC_HandleTypeDef * hdac, uint8_t dac_channel);
	dacplay_init(&spk_dacplay_data, spk_dacplay_data.id,  LOCAL_SPK_DAC_HANDLE, LOCAL_SPK_DAC_CHANNEL);
	if (local_spk_is_started())return;
    //res =
	if (res) 
	{
		res = dacplay_start_dma(&spk_dacplay_data);
	}
	//dacplay_start_dma(&spk_dacplay_data);
    //set_tim_frequency2(&htim15, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);
    HAL_TIM_Base_Start_IT(LOCAL_SPK_ADC_TIM_HANDLE);
	return res;
}
