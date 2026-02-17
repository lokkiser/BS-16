/*
 * local_spk.c
 *
 *  Created on: 19 янв. 2026 г.
 *      Author: Rafael Boltachev
 */


#include "local_spk.h"
#include <string.h>
#include "stm_util.h"

dacplay_data_t spk_dacplay_data = DACPLAY_STATIC_INIT(11, LOCAL_SPK_DAC_HANDLE, LOCAL_SPK_DAC_CHANNEL);
adcplay_data_t spk_adcplay_data = ADCPLAY_STATIC_INIT( 2, LOCAL_SPK_ADC_HANDLE);


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

void local_spk_adc_FullCpltCallback(void)
{
	adcplay_FullCpltCallback(&spk_adcplay_data);
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
	bool res;

	if (local_spk_is_started()) 
		local_spk_stop();

    res = adcplay_start_dma(&spk_adcplay_data);
	if (res) 
	{
		res = dacplay_start_dma(&spk_dacplay_data);
	}
	return res;
}

