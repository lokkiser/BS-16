/*
 * dacplaybuf_util.c
 *
 *  Created on: 19 февр. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "dacplaybuf_util.h"
#include "stm_util.h"

/*
typedef struct _dacplaybuf_data {

	DAC_HandleTypeDef * hdac;
	uint8_t dac_channel;
	TIM_HandleTypeDef * htim1;

	uint16_t dma_buf[2][DACPLAYBUF_DMA_BUF_SIZE];

	uint8_t dma_index = 0xFF;
	bool dma_started = false;

	uint8_t lwrb_data[dacplaybuf_LWRB_DATA_SIZE];
	lwrb_t lwrb;
	uint32_t lwrb_write_tick;

} dacplaybuf_data_t;


#define dacplaybuf_HDAC			hdac
#define dacplaybuf_DAC_CHANNEL		DAC_CHANNEL_1
#define dacplaybuf_HTIM			htim8

#define DACPLAYBUF_DMA_BUF_SIZE	640
#define dacplaybuf_LWRB_DATA_SIZE	6400


//буффер под DMA
static uint16_t dacplaybuf_dma_buf[2][DACPLAYBUF_DMA_BUF_SIZE];
static uint8_t dacplaybuf_dma_index = 0xFF;
static bool dacplaybuf_dma_started = false;
static uint32_t dacplaybuf_frequency = 8000;



static uint8_t dacplaybuf_lwrb_data[dacplaybuf_LWRB_DATA_SIZE];
static lwrb_t dacplaybuf_lwrb;
static uint32_t dacplaybuf_lwrb_write_tick = 0;
*/

//Событие по половине DMA
void dacplaybuf_HalfCpltCallback(dacplaybuf_data_t* dacplaybuf_data)
{
	dacplaybuf_data->dma_index = 0;
}

//Событие по целой DMA
void dacplaybuf_FullCpltCallback(dacplaybuf_data_t* dacplaybuf_data)
{
	dacplaybuf_data->dma_index = 1;
}


void dacplaybuf_stop_dma(dacplaybuf_data_t* dacplaybuf_data)
{
	HAL_DAC_Stop_DMA(dacplaybuf_data->hdac, dacplaybuf_data->dac_channel);
	//if (dacplaybuf_data->htim) HAL_TIM_Base_Stop(dacplaybuf_data->htim);
	dacplaybuf_data->dma_started = false;
	lwrb_reset(&dacplaybuf_data->lwrb);
	ulog_fmt("dacplay: stop dma (id:%d)"CRLF, dacplaybuf_data->id);
}

void dacplaybuf_start_dma(dacplaybuf_data_t* dacplaybuf_data)
{
	if (dacplaybuf_data->dma_started) dacplaybuf_stop_dma(dacplaybuf_data);
	if (dacplaybuf_data->htim) {
		set_tim_frequency2(dacplaybuf_data->htim, dacplaybuf_data->frequency, BASE_TIM_FREQUENSY);
		HAL_TIM_Base_Start(dacplaybuf_data->htim);
	}
	HAL_DAC_Start_DMA(dacplaybuf_data->hdac, dacplaybuf_data->dac_channel, (uint32_t*)dacplaybuf_data->dma_buf, DACPLAYBUF_DMA_BUF_SIZE * 2, DAC_ALIGN_12B_R);
	dacplaybuf_data->dma_started = true;
	ulog_fmt("dacplay: start dma frequency: %lu (id:%d)"CRLF, dacplaybuf_data->frequency, dacplaybuf_data->id);
}

bool dacplaybuf_is_dma_started(dacplaybuf_data_t* dacplaybuf_data)
{
	return dacplaybuf_data->dma_started;
}

//Установка частоты на DAC
void dacplaybuf_set_frequency(dacplaybuf_data_t* dacplaybuf_data, uint32_t Frequency)
{
	dacplaybuf_data->frequency = Frequency;
}

uint32_t dacplaybuf_get_frequency(dacplaybuf_data_t* dacplaybuf_data)
{
	return dacplaybuf_data->frequency;
}


void dacplaybuf_put(dacplaybuf_data_t * dacplaybuf_data, uint16_t* dac_data, uint32_t dac_data_len)
{
	uint8_t* tmp;

	if (!dac_data_len) return;

	if (lwrb_get_free(&dacplaybuf_data->lwrb) < dac_data_len * 2) {
		ulog_fmt("dacplay: buffer full (id:%d)"CRLF, dacplaybuf_data->id); //переполнение буфера
		tmp = (uint8_t *)malloc(dac_data_len * 2);
		lwrb_read(&dacplaybuf_data->lwrb, tmp, dac_data_len * 2);
		free(tmp);
	}

	lwrb_write(&dacplaybuf_data->lwrb, dac_data, dac_data_len * 2);


	if (!dacplaybuf_is_dma_started(dacplaybuf_data)) {
		if (lwrb_get_full(&dacplaybuf_data->lwrb) >= lwrb_get_free(&dacplaybuf_data->lwrb)) { //запуск DMA если данных в буфере достаточно
			dacplaybuf_start_dma(dacplaybuf_data);
		}
	}

	dacplaybuf_data->lwrb_write_tick = HAL_GetTickNoZero();
}

size_t dacplaybuf_get_free(dacplaybuf_data_t* dacplaybuf_data)
{
	return lwrb_get_free(&dacplaybuf_data->lwrb);
}

size_t dacplaybuf_get_full(dacplaybuf_data_t* dacplaybuf_data)
{
	return lwrb_get_full(&dacplaybuf_data->lwrb);
}

void dacplaybuf_reset(dacplaybuf_data_t* dacplaybuf_data)
{
	uint16_t i;

	lwrb_reset(&dacplaybuf_data->lwrb);

	for (i = 0; i < DACPLAYBUF_DMA_BUF_SIZE; i++) 
	{
		dacplaybuf_data->dma_buf[0][i] = 2047;
		dacplaybuf_data->dma_buf[1][i] = 2047;
	}
}

lwrb_t * dacplaybuf_get_lwrb(dacplaybuf_data_t * dacplaybuf_data)
{
	return &dacplaybuf_data->lwrb;
}

void dacplaybuf_fill(dacplaybuf_data_t * dacplaybuf_data)
{
	size_t rc;
	uint16_t i;


	rc = lwrb_read(&dacplaybuf_data->lwrb, dacplaybuf_data->dma_buf[dacplaybuf_data->dma_index], sizeof(dacplaybuf_data->dma_buf[dacplaybuf_data->dma_index]));

	if (rc < sizeof(dacplaybuf_data->dma_buf[dacplaybuf_data->dma_index])) 
	{
		for (i = rc / 2; i < DACPLAYBUF_DMA_BUF_SIZE; ++i) dacplaybuf_data->dma_buf[dacplaybuf_data->dma_index][i] = 2047;
		//ulog_fmt("dacplay: buffer empty (%d %lu) (id:%d)"CRLF, dacplaybuf_data->dma_index, rc, dacplaybuf_data->id);
	}

	dacplaybuf_data->dma_index = 0xFF;
}


//задача
void dacplaybuf_task(dacplaybuf_data_t* dacplaybuf_data)
{

	if (dacplaybuf_is_dma_started(dacplaybuf_data)) {

		if (dacplaybuf_data->dma_index < 2) {
			dacplaybuf_fill(dacplaybuf_data);
		}
	}


	if (dacplaybuf_data->lwrb_write_tick && (HAL_DiffTick(dacplaybuf_data->lwrb_write_tick) > 500)) { //если длительное время не писали в lwrb

		if (dacplaybuf_is_dma_started(dacplaybuf_data)) {
			if (!lwrb_get_full(&dacplaybuf_data->lwrb)) { //ждем освобождения буфера
				dacplaybuf_stop_dma(dacplaybuf_data);
				dacplaybuf_data->lwrb_write_tick  = 0;
				dacplaybuf_finish(dacplaybuf_data);
			}
		}
		else dacplaybuf_data->lwrb_write_tick = 0;
	}
}

void dacplaybuf_init(dacplaybuf_data_t * dacplaybuf_data, uint8_t id, DAC_HandleTypeDef * hdac, uint8_t dac_channel, TIM_HandleTypeDef * htim)
{
	dacplaybuf_data->id = id;
	dacplaybuf_data->hdac = hdac;
	dacplaybuf_data->dac_channel = dac_channel;
	dacplaybuf_data->htim = htim;



	lwrb_init(&dacplaybuf_data->lwrb, dacplaybuf_data->lwrb_data, sizeof(dacplaybuf_data->lwrb_data));
	dacplaybuf_data->dma_index = 0xFF;
	dacplaybuf_data->lwrb_write_tick = 0;
	dacplaybuf_data->dma_started = false;
}
