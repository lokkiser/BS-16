/*
 * mp3read.c
 *
 *  Created on: 6 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#include "mp3play_util.h"
#include <stdio.h>
#include "pcm_util.h"
#include "dacplay_util.h"
#include "fatfs_util.h"
#include "stm_util.h"
#include "str_util.h"
#include "mad_util.h"
#include "dac_util.h"

#ifndef MP3PLAY_MAX_AUDIO_FREQUENCY
#define MP3PLAY_MAX_AUDIO_FREQUENCY		44100
#endif


//аудио файл
//static FIL mp3play_file = {0};
#define mp3play_file f_global_file

//dac data
static dacplay_data_t * mp3play_dacplay_data = NULL;
//volume
static uint16_t mp3play_volume = 100;
//Tim
static TIM_HandleTypeDef * mp3play_htim = NULL;
//file is EOF
static bool mp3play_enable = false;
//samplerate
static unsigned int mp3play_samplerate = 0;

#ifndef MP3PLAY_DYNAMIC

//buffer
static lwrb_t mp3play_buffer = {0};

void mp3play_mad2buf(void)
{
	int rc;
	int16_t pcm16[MAX_MAD_PCM_SIZE];
	uint16_t * dac_data;



	rc = mad_read_next_frame(&global_mad_data);
	//ulog_fmt("r:%d ", rc);
	if (rc == -1) {
		f_close(&mp3play_file);
		ulog("mp3play: close MP3 file"CRLF);
		return;
	}
	if (rc != 1) return;

   	mad_synth_frame(&global_mad_data.synth, &global_mad_data.frame);
   	rc = mad_output_pcm16_mono(&global_mad_data.synth.pcm, pcm16, MAX_MAD_PCM_SIZE);
   	if (!rc) return;



   	if (global_mad_data.frame.header.samplerate) {


   		if (!mp3play_samplerate) {

   			mp3play_samplerate = MIN(global_mad_data.frame.header.samplerate, MP3PLAY_MAX_AUDIO_FREQUENCY);
   			set_tim_frequency2_and_start(mp3play_htim, mp3play_samplerate, BASE_TIM_FREQUENSY);
   			ulog_fmt("mp3play: samplerate=%lu, %s"CRLF, mp3play_samplerate, (global_mad_data.frame.header.mode == MAD_MODE_SINGLE_CHANNEL ? "mono" : "stereo"));
   		}


   		if (global_mad_data.frame.header.samplerate != mp3play_samplerate) {
   		   	rc = pcm_change_frequency2(pcm16, mp3play_samplerate, pcm16, rc, global_mad_data.frame.header.samplerate, 1);
   		   	if (!rc) return;
   		}

   	}


#ifdef MP3PLAY_ON_PCM_WRITE
   	mp3play_on_pcm_write(pcm16, rc);
#endif

   	if (mp3play_volume != 100) {
   		pcm_set_volume(pcm16, pcm16, rc, mp3play_volume);
   	}


   	dac_data = (uint16_t *)pcm16;
	pcm_mono2dac12(pcm16, dac_data, rc);

#ifdef MP3PLAY_ON_DAC_WRITE
   	mp3play_on_dac_write(dac_data, rc);
#endif

	lwrb_write(&mp3play_buffer, dac_data, rc * 2);
	//ulog_fmt("p:%d(%lu) ", rc * 2, lwrb_get_full(&mp3play_buffer));
}



void mp3play_stop(void)
{
	//ulog("510"CRLF);
	if (f_is_open(&mp3play_file)) {
		//ulog("520"CRLF);
		f_close(&mp3play_file);
		ulog("mp3play: close MP3 file"CRLF);
	}
	//ulog("530"CRLF);
	mad_data_clear(&global_mad_data);
	//ulog("540"CRLF);
	if (dacplay_is_dma_started(mp3play_dacplay_data)) dacplay_stop_dma(mp3play_dacplay_data); //здеся глючит
	//ulog("550"CRLF);
	lwrb_reset(&mp3play_buffer);
	//ulog("560"CRLF);
	mp3play_enable = false;
	mp3play_on_stop();
	//ulog("570"CRLF);
}


void mp3play_buf2dma(uint8_t index)
{
	int rc, i;
	//int rc;
	//uint16_t dma_buf[DACPLAY_DMA_BUF_SIZE];

	if (index > 1) return;

	//rc = lwrb_read(&mp3play_buffer, dma_buf, DACPLAY_DMA_BUF_SIZE * 2);


	rc = lwrb_read(&mp3play_buffer, mp3play_dacplay_data->dma_buf[index], DACPLAY_DMA_BUF_SIZE * 2);
	if (rc < DACPLAY_DMA_BUF_SIZE * 2) {
		for (i = rc / 2; i < DACPLAY_DMA_BUF_SIZE; ++i) mp3play_dacplay_data->dma_buf[index][i] = 2047;
	}
	//ulog_fmt("g:%d(%d, %lu) ", index, rc, lwrb_get_full(&mp3play_buffer));

}

void mp3play_task(void)
{
	if (!mp3play_enable) return;


	if (!dacplay_is_dma_started(mp3play_dacplay_data)) {
		mp3play_stop();
		return;
	}



	if (f_is_open(&mp3play_file)) {



		if (lwrb_get_full(&mp3play_buffer) >= DACPLAY_DMA_BUF_SIZE * 2) { //кол-во дынных в буфере больше чем в DMA
			if (mp3play_dacplay_data->index < 2) {
				mp3play_buf2dma(mp3play_dacplay_data->index);
				mp3play_dacplay_data->index = 0xFF;
			}
		}


		if (lwrb_get_free(&mp3play_buffer) >= MAX_MAD_PCM_SIZE * 2) {//свободного в буфере больше чем MAD PCM size
			mp3play_mad2buf();
		}


	} else {

		if (lwrb_get_full(&mp3play_buffer)) {
			if (mp3play_dacplay_data->index < 2) {
				mp3play_buf2dma(mp3play_dacplay_data->index);
				mp3play_dacplay_data->index = 0xFF;
			}
		}
		else {
			mp3play_stop();
		}

	}



}


bool mp3play(const char * file_name, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim)
{


	if (!strlen(file_name)) return false;

	if (f_is_open(&mp3play_file)) {
		f_close(&mp3play_file);
	}
	mad_data_clear(&global_mad_data);
	if (dacplay_is_dma_started(dacplay_data)) dacplay_stop_dma(dacplay_data);
	lwrb_reset(&mp3play_buffer);


	if (f_open(&mp3play_file, file_name, FA_READ) != FR_OK) {
	     ulog_fmt("mp3play: error open file: %s"CRLF, file_name);
	     return false;
	}


	global_mad_data.file = &mp3play_file;
	global_mad_data.mad_input_read = f_stream_read;
	mad_stream_init(&global_mad_data.stream);
	mad_frame_init(&global_mad_data.frame);
	global_mad_data.frame.header.samplerate = 0;
	mad_synth_init(&global_mad_data.synth);
	global_mad_data.synth.pcm.length = 0;
	mad_stream_options(&global_mad_data.stream, 0); // TODO - add options support


	mp3play_dacplay_data = dacplay_data;
	mp3play_volume = volume;
	mp3play_htim = htim;
	mp3play_enable = true;
	mp3play_samplerate = 0;

	lwrb_reset(&mp3play_buffer);

	for (uint16_t i = 0; i < DACPLAY_DMA_BUF_SIZE; ++i) {
		mp3play_dacplay_data->dma_buf[0][i] = 2047;
		mp3play_dacplay_data->dma_buf[1][i] = 2047;
	}


	set_tim_frequency2_and_start(mp3play_htim, MP3PLAY_MAX_AUDIO_FREQUENCY, BASE_TIM_FREQUENSY);
	dacplay_start_dma(mp3play_dacplay_data);


    ulog_fmt("mp3play: play file:%s volume:%d play_on:%d"CRLF, file_name, mp3play_volume, mp3play_dacplay_data->id);
	return true;
}

void mp3play_init(uint8_t * buf, uint16_t buf_size)
{
	lwrb_init(&mp3play_buffer, buf, buf_size);
}

#endif //VERSION 1
//---------------------------------------------------------

#ifdef MP3PLAY_DYNAMIC

#include "mp3read_util.h"
#include "dynbuf_util.h"



//dac data
static dynamic_buf_t mp3play_buf = DYNAMIC_BUF_STATIC_INIT(MAX_MAD_PCM_SIZE * 4);
static mad_data_t * pmad_data = NULL;

void mp3play_stop(void)
{
	if (f_is_open(&mp3play_file)) {
		f_close(&mp3play_file);
		ulog("mp3play: close MP3 file"CRLF);
	}
	if (pmad_data) {
		mad_data_clear(pmad_data);
		free(pmad_data);
		pmad_data = NULL;
	}

	if (dacplay_is_dma_started(mp3play_dacplay_data)) dacplay_stop_dma(mp3play_dacplay_data);
	dynamic_buf_clear(&mp3play_buf);
	mp3play_enable = FALSE;
	mp3play_on_stop();
}



void mp3play_mad2buf(void)
{
	int rc;
	int16_t pcm16[MAX_MAD_PCM_SIZE];
	uint16_t * dac_data;


	rc =  mp3read_read_pcm16_mono(&mp3play_file, pmad_data, pcm16);

	if (rc == -1) {
		f_close(&mp3play_file);
		ulog("mp3play: close MP3 file"CRLF);
		return;
	}


   	if (pmad_data->frame.header.samplerate) {


   		if (!mp3play_samplerate) {

   			mp3play_samplerate = MIN(pmad_data->frame.header.samplerate, MP3PLAY_MAX_AUDIO_FREQUENCY);
   			set_tim_frequency2_and_start(mp3play_htim, mp3play_samplerate, BASE_TIM_FREQUENSY);
   			ulog_fmt("mp3play: samplerate=%lu, %s"CRLF, mp3play_samplerate, (pmad_data->frame.header.mode == MAD_MODE_SINGLE_CHANNEL ? "mono" : "stereo"));
   		}


   		if (pmad_data->frame.header.samplerate != mp3play_samplerate) {
   		   	rc = pcm_change_frequency2(pcm16, mp3play_samplerate, pcm16, rc, pmad_data->frame.header.samplerate, 1);
   		   	if (!rc) return;
   		}

   	}


#ifdef MP3PLAY_ON_PCM_WRITE
   	mp3play_on_pcm_write(pcm16, rc);
#endif

   	if (mp3play_volume != 100) {
   		pcm_set_volume(pcm16, pcm16, rc, mp3play_volume);
   	}


   	dac_data = (uint16_t *)pcm16;
	pcm_mono2dac12(pcm16, dac_data, rc);

#ifdef MP3PLAY_ON_DAC_WRITE
   	mp3play_on_dac_write(dac_data, rc);
#endif

	dynamic_buf_push(&mp3play_buf, dac_data, rc * 2);
}

void mp3play_buf2dma(void)
{
	int i;
	size_t data_len;

	if (mp3play_dacplay_data->index > 1) return;

	if (!dynamic_buf_pop_line(&mp3play_buf, mp3play_dacplay_data->dma_buf[mp3play_dacplay_data->index], DACPLAY_DMA_BUF_SIZE * 2, &data_len)) return;

	if (data_len < DACPLAY_DMA_BUF_SIZE * 2) {
		for (i = data_len / 2; i < DACPLAY_DMA_BUF_SIZE; ++i) mp3play_dacplay_data->dma_buf[mp3play_dacplay_data->index][i] = 2047;
	}
}

void mp3play_task(void)
{
	if (!mp3play_enable) return;


	if (!dacplay_is_dma_started(mp3play_dacplay_data)) {
		mp3play_stop();
		return;
	}



	if (f_is_open(&mp3play_file)) {



		if (dynamic_buf_get_data_size(&mp3play_buf) >= DACPLAY_DMA_BUF_SIZE * 2) { //кол-во дынных в буфере больше чем в DMA
			if (mp3play_dacplay_data->index < 2) {
				mp3play_buf2dma();
				mp3play_dacplay_data->index = 0xFF;
			}
		}


		if (dynamic_buf_get_free_size(&mp3play_buf) >= MAX_MAD_PCM_SIZE * 2) {//свободного в буфере больше чем MAD PCM size
			mp3play_mad2buf();
		}


	} else {

		if (dynamic_buf_is_empty(&mp3play_buf)) {
			mp3play_stop();
		} else {
			if (mp3play_dacplay_data->index < 2) {
				mp3play_buf2dma();
				mp3play_dacplay_data->index = 0xFF;
			}
		}
	}



}


bool mp3play(const char * file_name, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim)
{

	int i;

	if (!strlen(file_name)) return FALSE;

	if (f_is_open(&mp3play_file)) {
		f_close(&mp3play_file);
	}
	if (pmad_data) {
		mad_data_clear(pmad_data);
		free(pmad_data);
	}
	if (dacplay_is_dma_started(mp3play_dacplay_data)) {
		dacplay_stop_dma(mp3play_dacplay_data);
	}
	dynamic_buf_clear(&mp3play_buf);
	mp3play_enable = FALSE;




	pmad_data = (mad_data_t *)malloc(sizeof(mad_data_t));
	if (!pmad_data) return FALSE;

	if (!mp3read_open(&mp3play_file, pmad_data, file_name)) return FALSE;


	mp3play_dacplay_data = dacplay_data;
	mp3play_volume = volume;
	mp3play_htim = htim;
	mp3play_enable = TRUE;
	mp3play_samplerate = 0;


	for (i = 0; i < DACPLAY_DMA_BUF_SIZE; ++i) {
		mp3play_dacplay_data->dma_buf[0][i] = 2047;
		mp3play_dacplay_data->dma_buf[1][i] = 2047;
	}


	set_tim_frequency2_and_start(mp3play_htim, MP3PLAY_MAX_AUDIO_FREQUENCY, BASE_TIM_FREQUENSY);
	dacplay_start_dma(mp3play_dacplay_data);


    ulog_fmt("mp3play2: play file:%s volume:%d play_on:%d"CRLF, file_name, mp3play_volume, mp3play_dacplay_data->id);
	return TRUE;
}

void mp3play_init(uint8_t * buf, uint16_t buf_size)
{
	dynamic_buf_init(&mp3play_buf,  buf_size);
}


#endif
