/*
 * dacplay_util.c
 *
 *  Created on: 20 мая 2024 г.
 *      Author: Rafael Boltachev
 */


#include "dacplay_util.h"
#include "stm_util.h"


//Событие по половине DMA
void dacplay_HalfCpltCallback(dacplay_data_t * dacplay_data)
{
	if (!dacplay_data) return;
	dacplay_data->index = 0;
#ifdef USE_DACPLAY_READ_DATA
	dacplay_read_data(dacplay_data, dacplay_data->dma_buf[0]);
#endif

}

//Событие по целой DMA
void dacplay_FullCpltCallback(dacplay_data_t * dacplay_data)
{
	if (!dacplay_data) return;
	dacplay_data->index = 1;
#ifdef USE_DACPLAY_READ_DATA
	dacplay_read_data(dacplay_data, dacplay_data->dma_buf[1]);
#endif
}

void dacplay_fill_zero(dacplay_data_t * dacplay_data)
{
	uint16_t i, j;

	if (!dacplay_data) return;

	for (i = 0; i < 2; ++i) {
		for (j = 0; j < DACPLAY_DMA_BUF_SIZE; ++j) {
			dacplay_data->dma_buf[i][j] = DACPLAY_ZERO_VALUE;
		}
	}
}


//#ifndef DACPLAY_VERSION_DYNAMIC



void dacplay_stop_dma(dacplay_data_t * dacplay_data)
{
	if (!dacplay_data) return;

	dacplay_fill_zero(dacplay_data);
	HAL_DAC_Stop_DMA(dacplay_data->hdac, dacplay_data->dac_channel);
	dacplay_data->index = 0xFF;
	dacplay_data->dma_started = false;
	ulog_fmt("dacplay: stop dma (id:%d)"CRLF, dacplay_data->id);
}

bool dacplay_start_dma(dacplay_data_t * dacplay_data)
{
	HAL_StatusTypeDef status;

	if (!dacplay_data) return false;

	if (dacplay_data->dma_started) dacplay_stop_dma(dacplay_data);
	dacplay_fill_zero(dacplay_data);
	status = HAL_DAC_Start_DMA(dacplay_data->hdac, dacplay_data->dac_channel, (uint32_t * )dacplay_data->dma_buf, DACPLAY_DMA_BUF_SIZE * 2, DAC_ALIGN_12B_R);
	if (status == HAL_OK) {
		dacplay_data->dma_started = true;
		ulog_fmt("dacplay: start dma (id:%d)"CRLF, dacplay_data->id);
		return true;
	} else {
		ulog_fmt("dacplay: invalid start dma (id:%d)"CRLF, dacplay_data->id);
		return false;
	}
}

bool dacplay_is_dma_started(dacplay_data_t * dacplay_data)
{
	if (!dacplay_data) return false;
	return dacplay_data->dma_started;
}



void dacplay_init(dacplay_data_t * dacplay_data, uint8_t id, DAC_HandleTypeDef * hdac, uint8_t dac_channel)
{
	if (!dacplay_data) return;

	dacplay_data->id = id;
	dacplay_data->hdac = hdac;
	dacplay_data->dac_channel = dac_channel;
	dacplay_data->dma_started = false;
	dacplay_data->index = 0xFF;
}

//#endif


// ---------------------------------------------------------------------------------------------------------
/*
#ifdef DACPLAY_VERSION_DYNAMIC


void dacplay_init(dacplay_data_t * dacplay_data, uint8_t id, DAC_HandleTypeDef * hdac, uint8_t dac_channel)
{
	//uint32_t buf_size =  * 1024;

	if (!dacplay_data) return;
	dacplay_data->id = id;
	dacplay_data->hdac = hdac;
	dacplay_data->dac_channel = dac_channel;
	dacplay_data->index = 0xFF;
	dacplay_data->buf = NULL;
	dacplay_data->dma_buf[0] = NULL;
	dacplay_data->dma_buf[1] = NULL;
}

void dacplay_stop_dma(dacplay_data_t * dacplay_data)
{
	if (!dacplay_data) return;
	dacplay_fill_zero(dacplay_data);
	HAL_DAC_Stop_DMA(dacplay_data->hdac, dacplay_data->dac_channel);
	dacplay_data->index = 0xFF;
	if (dacplay_data->buf) {
		free(dacplay_data->buf);
		dacplay_data->buf = NULL;
		dacplay_data->dma_buf[0] = NULL;
		dacplay_data->dma_buf[1] = NULL;
	}
	ulog_fmt("dacplay2: stop dma (id:%d)"CRLF, dacplay_data->id);

}


bool dacplay_start_dma(dacplay_data_t * dacplay_data)
{
	if (!dacplay_data) return false;
	if (dacplay_data->buf) dacplay_stop_dma(dacplay_data);
	dacplay_data->buf  = (uint8_t *)malloc(DACPLAY_DMA_BUF_SIZE * 4);
	if (!dacplay_data->buf) return false;
	dacplay_data->dma_buf[0] = (uint16_t *)dacplay_data->buf;
	dacplay_data->dma_buf[1] = (uint16_t *)(dacplay_data->buf + DACPLAY_DMA_BUF_SIZE * 2);
	HAL_DAC_Start_DMA(dacplay_data->hdac, dacplay_data->dac_channel, (uint32_t * )dacplay_data->buf, DACPLAY_DMA_BUF_SIZE * 2, DAC_ALIGN_12B_R);
	ulog_fmt("dacplay2: start dma (id:%d)"CRLF, dacplay_data->id);
	return true;
}

bool dacplay_is_dma_started(dacplay_data_t * dacplay_data)
{
	return dacplay_data->buf ? true : false;
}


#endif

*/
