/*
 * memplay.c
 *
 *  Created on: 8 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#include "memplay_util.h"

#include <stdio.h>
#include "mad_util.h"
#include "ff.h"
#include "pcm_util.h"
//#include "audio.h"
#include "dacplay_util.h"
#include "law_util.h"
#include "fatfs_util.h"
#include "stm_util.h"
#include "str_util.h"
#include "memplay_util.h"
#include "dac_util.h"

#ifndef MEMPLAY_MAX_AUDIO_FREQUENCY
#define MEMPLAY_MAX_AUDIO_FREQUENCY		44100
#endif


//dac data
static dacplay_data_t * memplay_dacplay_data = NULL;
//volume
static uint16_t memplay_volume = 100;
//tim
static TIM_HandleTypeDef * memplay_htim = NULL;
//memplay enable
static bool memplay_enable = FALSE;
//size
static uint32_t memplay_size = 0;
//mem
static uint8_t * memplay_mem = NULL;
//samplerate
static unsigned int memplay_samplerate = 0;



unsigned int memplay_input_read(void * file, void * read_start, unsigned int read_size)
{
	size_t read;

	if (!memplay_size) return 0;

	read = MIN(memplay_size, read_size);

	memcpy(read_start, memplay_mem, read);
	memplay_mem += read;
	memplay_size -= read;

	return read;
}


#ifndef MEMPLAY_DYNAMIC

#include "lwrb_util.h"

//buffer
static lwrb_t memplay_buffer = {0};

void memplay_mad2buf(void)
{
	int rc;
	int16_t pcm16[MAX_MAD_PCM_SIZE];
	uint16_t * dac_data;

	rc = mad_read_next_frame(&global_mad_data);
	//ulog_fmt("r:%d ", rc);
	if (rc == -1) 
	{
		ulog("memplay: MEM is zero"CRLF);
		return;
	}
	if (rc != 1) return;

   	mad_synth_frame(&global_mad_data.synth, &global_mad_data.frame);
   	rc = mad_output_pcm16_mono(&global_mad_data.synth.pcm, pcm16, MAX_MAD_PCM_SIZE);
   	if (!rc) return;

   	if (global_mad_data.frame.header.samplerate) 
   	{
   		if (!memplay_samplerate) 
   		{
   			memplay_samplerate = MIN(global_mad_data.frame.header.samplerate, MEMPLAY_MAX_AUDIO_FREQUENCY);
   			set_tim_frequency2_and_start(memplay_htim, memplay_samplerate, BASE_TIM_FREQUENSY);
   			ulog_fmt("memplay: samplerate=%lu, %s"CRLF, memplay_samplerate, (global_mad_data.frame.header.mode == MAD_MODE_SINGLE_CHANNEL ? "mono" : "stereo"));
   		}

   		if (global_mad_data.frame.header.samplerate != memplay_samplerate) 
   		{
   		   	rc = pcm_change_frequency(pcm16, memplay_samplerate, pcm16, rc, global_mad_data.frame.header.samplerate, 1);
   		   	if (!rc) return;
   		}
   	}

   	if (memplay_volume != 100) 
   	{
   		pcm_set_volume(pcm16, pcm16, rc, memplay_volume);
   	}

   	dac_data = (uint16_t *)pcm16;
	pcm_mono2dac12(pcm16, dac_data, rc);

	lwrb_write(&memplay_buffer, dac_data, rc * 2);
	//ulog_fmt("p:%d(%lu) ", rc * 2, lwrb_get_full(&memplay_buffer));
}

void memplay_stop(void)
{
	if (memplay_size) 
	{
		ulog("memplay: abort"CRLF);
		memplay_size = 0;
	}
	mad_data_clear(&global_mad_data);
	
	if (dacplay_is_dma_started(memplay_dacplay_data)) 
		dacplay_stop_dma(memplay_dacplay_data);

	lwrb_reset(&memplay_buffer);
	memplay_enable = FALSE;
	memplay_on_stop();
}

void memplay_buf2dma(uint8_t index)
{
	int rc, i;

	if (index > 1) return;

	rc = lwrb_read(&memplay_buffer, memplay_dacplay_data->dma_buf[index], DACPLAY_DMA_BUF_SIZE * 2);
	if (rc < DACPLAY_DMA_BUF_SIZE * 2) 
	{
		for (i = rc / 2; i < DACPLAY_DMA_BUF_SIZE; ++i) memplay_dacplay_data->dma_buf[index][i] = 2047;
	}
	//ulog_fmt("g:%d(%d %lu) ", index, rc, lwrb_get_full(&memplay_buffer));
}

void memplay_task(void)
{
	if (!memplay_enable) return;

	if (!dacplay_is_dma_started(memplay_dacplay_data))
	{
		memplay_stop();
		return;
	}

	if (memplay_size) 
	{
		if (lwrb_get_full(&memplay_buffer) >= DACPLAY_DMA_BUF_SIZE * 2) //кол-во байт в буфере больше или равно чем байт в DMA
		{ 
			if (memplay_dacplay_data->index < 2) 
			{
				memplay_buf2dma(memplay_dacplay_data->index);
				memplay_dacplay_data->index = 0xFF;
			}
		}

		if (lwrb_get_free(&memplay_buffer) >= MAX_MAD_PCM_SIZE * 2)  //свободного в буфере больше чем MAD PCM size. Можно писать в буфер?
		{
			memplay_mad2buf();
		}
	} 
	else
	{
		if (lwrb_get_full(&memplay_buffer)) 
		{
			if (memplay_dacplay_data->index < 2) 
			{
				memplay_buf2dma(memplay_dacplay_data->index);
				memplay_dacplay_data->index = 0xFF;
			}
		}
		else 
		{
			ulog("memplay: stop"CRLF);
			memplay_stop();
		}
	}
}

//проигрывание mp3 файла из памяти
void memplay(const uint8_t * mem, uint32_t mem_size, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim)
{
	mad_data_clear(&global_mad_data);
	if (dacplay_is_dma_started(dacplay_data)) dacplay_stop_dma(dacplay_data);
	lwrb_reset(&memplay_buffer);


	global_mad_data.file = NULL;
	global_mad_data.mad_input_read = memplay_input_read;
	mad_stream_init(&global_mad_data.stream);
	mad_frame_init(&global_mad_data.frame);
	global_mad_data.frame.header.samplerate = 0;
	mad_synth_init(&global_mad_data.synth);
	global_mad_data.synth.pcm.length = 0;
	mad_stream_options(&global_mad_data.stream, 0); // TODO - add options support



	memplay_dacplay_data = dacplay_data;
	memplay_volume = volume;
	memplay_htim = htim;


	set_tim_frequency2_and_start(memplay_htim, MEMPLAY_MAX_AUDIO_FREQUENCY, BASE_TIM_FREQUENSY);
	dacplay_start_dma(memplay_dacplay_data);

	memplay_mem = (uint8_t *)mem;
	memplay_size = mem_size;
	memplay_enable = TRUE;
	memplay_samplerate = 0;

    ulog_fmt("memplay: play MEM mem_size:%lu volume:%d play_on:%d"CRLF, mem_size, memplay_volume, dacplay_data->id);
}

void memplay_init(uint8_t * buf, uint16_t buf_size)
{
	lwrb_init(&memplay_buffer, buf, buf_size);
}

#endif


#ifdef MEMPLAY_DYNAMIC

#include "dynbuf_util.h"

static dynamic_buf_t memplay_buf = DYNAMIC_BUF_STATIC_INIT(MAX_MAD_PCM_SIZE * 5);
static mad_data_t * pmad_data = NULL;


void memplay_stop(void)
{
	if (memplay_size) {
		ulog("memplay: abort"CRLF);
		memplay_size = 0;
	}
	if (pmad_data) {
		mad_data_clear(pmad_data);
		free(pmad_data);
		pmad_data = NULL;
	}
	if (dacplay_is_dma_started(memplay_dacplay_data)) {
		dacplay_stop_dma(memplay_dacplay_data);
		ulog("memplay_on_stop"CRLF);
	}
	dynamic_buf_clear(&memplay_buf);
	memplay_enable = FALSE;
	memplay_on_stop();
}



void memplay_buf2dma(void)
{
	int i;
	size_t data_len;

	if (memplay_dacplay_data->index > 1) return;

	if (!dynamic_buf_pop_line(&memplay_buf, memplay_dacplay_data->dma_buf[memplay_dacplay_data->index], DACPLAY_DMA_BUF_SIZE * 2, &data_len)) return;

	if (data_len < DACPLAY_DMA_BUF_SIZE * 2) {
		for (i = data_len / 2; i < DACPLAY_DMA_BUF_SIZE; ++i) memplay_dacplay_data->dma_buf[memplay_dacplay_data->index][i] = 2047;
	}

	//ulog_fmt("pop %lu(%lu) ", data_len, dynamic_buf_get_data_size(&memplay_buf));

}


void memplay_mad2buf(void)
{
	int rc;
	int16_t pcm16[MAX_MAD_PCM_SIZE];
	uint16_t * dac_data;


	rc = mad_read_next_frame(pmad_data);
	//ulog_fmt("r:%d ", rc);
	if (rc == -1) {
		ulog("memplay: MEM is zero"CRLF);
		return;
	}
	if (rc != 1) return;

   	mad_synth_frame(&pmad_data->synth, &pmad_data->frame);
   	rc = mad_output_pcm16_mono(&pmad_data->synth.pcm, pcm16, MAX_MAD_PCM_SIZE);
   	if (!rc) return;



   	if (pmad_data->frame.header.samplerate) {

   		if (!memplay_samplerate) {

   			memplay_samplerate = MIN(pmad_data->frame.header.samplerate, MEMPLAY_MAX_AUDIO_FREQUENCY);
   			set_tim_frequency2_and_start(memplay_htim, memplay_samplerate, BASE_TIM_FREQUENSY);
   			ulog_fmt("memplay: samplerate=%lu, %s"CRLF, memplay_samplerate, (pmad_data->frame.header.mode == MAD_MODE_SINGLE_CHANNEL ? "mono" : "stereo"));

   		}


   		if (pmad_data->frame.header.samplerate != memplay_samplerate) {
   		   	rc = pcm_change_frequency(pcm16, memplay_samplerate, pcm16, rc, pmad_data->frame.header.samplerate, 1);
   		   	if (!rc) return;
   		}

   	}



   	if (memplay_volume != 100) {
   		pcm_set_volume(pcm16, pcm16, rc, memplay_volume);
   	}

   	dac_data = (uint16_t *)pcm16;
	pcm_mono2dac12(pcm16, dac_data, rc);

	dynamic_buf_push(&memplay_buf, dac_data, rc * 2);
	//ulog_fmt("push %d(%lu) ", rc * 2, dynamic_buf_get_data_size(&memplay_buf));

}

void memplay_task(void)
{
	if (!memplay_enable) return;

	if (!dacplay_is_dma_started(memplay_dacplay_data)){
		memplay_stop();
		return;
	}



	if (memplay_size) {



		if (dynamic_buf_get_data_size(&memplay_buf) >= DACPLAY_DMA_BUF_SIZE * 2) { //кол-во байт в буфере больше или равно чем байт в DMA
			if (memplay_dacplay_data->index < 2) {
				memplay_buf2dma();
				memplay_dacplay_data->index = 0xFF;
			}
		}


		if (dynamic_buf_get_free_size(&memplay_buf) >= MAX_MAD_PCM_SIZE * 2) {//свободного в буфере больше чем MAD PCM size. Можно писать в буфер?
			memplay_mad2buf();
		}


	} else {

		if (dynamic_buf_is_empty(&memplay_buf)) {
			ulog("memplay: buffer empty"CRLF);
			memplay_stop();
		}
		else {
			if (memplay_dacplay_data->index < 2) {
				memplay_buf2dma();
				memplay_dacplay_data->index = 0xFF;
			}
		}

	}


}


void memplay(const uint8_t * mem, uint32_t mem_size, dacplay_data_t * dacplay_data, uint16_t volume, TIM_HandleTypeDef * htim)
{


	if (pmad_data) {
		mad_data_clear(pmad_data);
		free(pmad_data);
	}
	if (dacplay_is_dma_started(memplay_dacplay_data)) {
		dacplay_stop_dma(memplay_dacplay_data);
	}
	dynamic_buf_clear(&memplay_buf);



	pmad_data = (mad_data_t *)malloc(sizeof(mad_data_t));
	if (!pmad_data) return;

	pmad_data->file = NULL;
	pmad_data->mad_input_read = memplay_input_read;
	mad_stream_init(&pmad_data->stream);
	mad_frame_init(&pmad_data->frame);
	pmad_data->frame.header.samplerate = 0;
	mad_synth_init(&pmad_data->synth);
	pmad_data->synth.pcm.length = 0;
	mad_stream_options(&pmad_data->stream, 0); // TODO - add options support



	memplay_dacplay_data = dacplay_data;
	memplay_volume = volume;
	memplay_htim = htim;


	set_tim_frequency2_and_start(memplay_htim, MEMPLAY_MAX_AUDIO_FREQUENCY, BASE_TIM_FREQUENSY);
	dacplay_start_dma(memplay_dacplay_data);

	memplay_mem = (uint8_t *)mem;
	memplay_size = mem_size;
	memplay_enable = TRUE;
	memplay_samplerate = 0;

    ulog_fmt("memplay: play MEM mem_size:%lu volume:%d play_on:%d"CRLF, mem_size, memplay_volume, dacplay_data->id);
}

void memplay_init(uint8_t * buf, uint16_t buf_size)
{
	dynamic_buf_init(&memplay_buf,  buf_size);
}
#endif
