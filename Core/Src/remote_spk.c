/*
 * remote_spk.c
 *
 *  Created on: 19 янв. 2026 г.
 *      Author: Rafael Boltachev
 */

#include "main.h"
#include "remote_spk.h"
#include <string.h>
#include "stm_util.h"
#include "TAS5720.h"


static uint16_t silence_buffer[64] = {0};
uint32_t sai_buffer[TOTAL_SAMPLES * TDM_CHANNELS];

/*void remote_spk_dac_HalfCpltCallback(void)
{
	dacplay_HalfCpltCallback(&spk_dacplay_data);
	//fulog("d0"CRLF);
}

void remote_spk_dac_FullCpltCallback(void)
{
	dacplay_FullCpltCallback(&spk_dacplay_data);
	//fulog("d1"CRLF);
}

void remote_spk_adc_HalfCpltCallback(void)
{
	adcplay_HalfCpltCallback(&spk_adcplay_data);
	//fulog("a0"CRLF);
}

void remote_spk_adc_FullCpltCallback(void) //тангента
{

	//должен писать в буфер
	//adcplay_FullCpltCallback(&spk_adcplay_data);
	//fulog("a1"CRLF);
}

bool remote_spk_is_started(void)
{
	return dacplay_is_dma_started(&spk_dacplay_data) || adcplay_is_dma_started(&spk_adcplay_data);
}
*/
void remote_spk_stop(void)
{
	//adcplay_stop_dma(&spk_adcplay_data);
	//dacplay_stop_dma(&spk_dacplay_data);
	Audio_System_Stop(remote_SPK_SAI_HANDLE);
}

bool remote_spk_start(void)
{
	bool res;
	/*
	status = Audio_System_Start( , uint16_t _len);
	if (status != HAL_OK)
	{
		local_mic_stop();
		ulog_fmt("local_mic: error invalid start dma DAC status = %d"CRLF, status);
		return false;
	}

	local_mic_is_start = true;
	ulog("local_mic: start dma"CRLF);

	if (remote_spk_is_started())
		remote_spk_stop();

	*/
    res = Audio_System_Start(remote_SPK_SAI_HANDLE, sai_buffer, silence_buffer, TOTAL_SAMPLES * TDM_CHANNELS);
    //set_tim_frequency2(&htim15, BASE_AUDIO_FREQUENCY, BASE_FREQUENCY);
    //
	return res;
}
