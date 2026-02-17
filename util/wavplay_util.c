/*
 * wavplay_util.c
 *
 *  Created on: 22 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#include "wavplay_util.h"
#include "fatfs_util.h"
#include "wav_util.h"
#include "dacplay_util.h"
#include "stm_util.h"

//аудио файл
static FIL wavplay_file = {0};
//dac data
static dacplay_data_t * wavplay_dacplay_data = NULL;
//volume
static uint16_t wavplay_volume = 100;
//wav header
static wav_header_t wavplay_wav_hdr = {0};

void wavplay_stop(void)
{
	if (f_is_open(&wavplay_file)) {
		f_close(&wavplay_file);
		ulog("wavplay: close WAV file"CRLF);
	}
	if (dacplay_is_dma_started(wavplay_dacplay_data)) {
		dacplay_stop_dma(wavplay_dacplay_data);
	}
	wavplay_on_stop();
}


void wavplay_task(void)
{
	uint16_t rc, i;

	if (!f_is_open(&wavplay_file)) return;

	if (!dacplay_is_dma_started(wavplay_dacplay_data)) {
		wavplay_stop();
		return;
	}

	if (wavplay_dacplay_data->index < 2) {
		rc = wavfile2dacdata(&wavplay_file, wavplay_dacplay_data->dma_buf[wavplay_dacplay_data->index], DACPLAY_DMA_BUF_SIZE, wavplay_wav_hdr.bits_per_sample, wavplay_wav_hdr.channel, wavplay_volume);
		if (rc) {
			if (rc < DACPLAY_DMA_BUF_SIZE) {
				for (i = rc; i < DACPLAY_DMA_BUF_SIZE; ++i) wavplay_dacplay_data->dma_buf[wavplay_dacplay_data->index][i] = 2047;
			}
			wavplay_dacplay_data->index = 0xFF;
		}
		else {
			wavplay_stop();
		}

	}
}



//проигрывание wav файла с флэшки
bool wavplay(const char * file_name, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim)
{
	uint8_t rc;


	wavplay_stop();


	if (!strlen(file_name)) return FALSE;

	rc = wavopenfile(file_name, &wavplay_file, &wavplay_wav_hdr);

	if (rc != WAV_OK) {
		ulog_fmt("wavplay: error WAV file %s, res:%s"CRLF, file_name, rc);
		return FALSE;
	}

	wavplay_dacplay_data = dacplay_data;
	wavplay_volume = volume;


	set_tim_frequency2_and_start(htim, wavplay_wav_hdr.sampling_f, BASE_TIM_FREQUENSY);
	dacplay_start_dma(wavplay_dacplay_data);


	ulog_fmt("wavplay: play WAV file %s (bit:%d, chan:%d, freq:%ld, play_on:%d)"CRLF, file_name, wavplay_wav_hdr.bits_per_sample, wavplay_wav_hdr.channel, wavplay_wav_hdr.sampling_f, wavplay_dacplay_data->id);

	return TRUE;
}

