/*
 * adcplay_util.c
 *
 *  Created on: 20 мая 2024 г.
 *      Author: Rafael Boltachev
 */


#include "adcplay_util.h"
#include "stm_util.h"


void adcplay_HalfCpltCallback(adcplay_data_t* adcplay_data)
{
	adcplay_data->index = 0;
#ifdef USE_ADCPLAY_WRITE_DATA
	adcplay_write_data(adcplay_data, adcplay_data->dma_buf[0]);
#endif
}

void adcplay_FullCpltCallback(adcplay_data_t* adcplay_data)
{
	adcplay_data->index = 1;
#ifdef USE_ADCPLAY_WRITE_DATA
	adcplay_write_data(adcplay_data, adcplay_data->dma_buf[1]);
#endif
}

//#ifndef ADCPLAY_VERSION_DYNAMIC

void adcplay_stop_dma(adcplay_data_t* adcplay_data)
{
	if (!adcplay_data) return;
	HAL_ADC_Stop_DMA(adcplay_data->hadc);
	adcplay_data->dma_start = false;
	adcplay_data->index = 0xFF;
	ulog_fmt("adcplay: stop dma (id:%d)"CRLF, adcplay_data->id);
}

bool adcplay_is_dma_started(adcplay_data_t* adcplay_data)
{
	if (!adcplay_data) return false;
	return adcplay_data->dma_start;
}

bool adcplay_start_dma(adcplay_data_t* adcplay_data)
{
	HAL_StatusTypeDef hs;

	if (!adcplay_data) return false;
	if (adcplay_data->dma_start) adcplay_stop_dma(adcplay_data);
	adcplay_data->index = 0xFF;
	hs = HAL_ADC_Start_DMA(adcplay_data->hadc, (uint32_t*)adcplay_data->dma_buf, ADCPLAY_DMA_BUF_SIZE * 2);
	if (hs != HAL_OK) 
	{
		ulog_fmt("adcplay: error start dma(id:%d)=%d"CRLF, adcplay_data->id, hs);
		return false;
	}
	adcplay_data->dma_start = true;
	ulog_fmt("adcplay: start dma(id:%d)=%d"CRLF, adcplay_data->id, hs);
	return true;
}

void adcplay_init(adcplay_data_t* adcplay_data, uint8_t id, ADC_HandleTypeDef* hadc)
{
	if (!adcplay_data) return;
	adcplay_data->id = id;
	adcplay_data->dma_start = false;
	adcplay_data->hadc = hadc;
	adcplay_data->index = 0xFF;
}

//#endif

/*
#ifdef ADCPLAY_VERSION_DYNAMIC


void adcplay_stop_dma(adcplay_data_t * adcplay_data)
{
	if (!adcplay_data) return;
	HAL_ADC_Stop_DMA(adcplay_data->hadc);
	adcplay_data->index = 0xFF;
	if (adcplay_data->buf) {
		free(adcplay_data->buf);
		adcplay_data->buf = NULL;
		adcplay_data->dma_buf[0] = NULL;
		adcplay_data->dma_buf[1] = NULL;
	}
	ulog_fmt("adcplay: stop dma (id:%d)"CRLF, adcplay_data->id);
}


bool adcplay_is_dma_started(adcplay_data_t * adcplay_data)
{
	return adcplay_data->buf ? true : false;
}

bool adcplay_start_dma(adcplay_data_t * adcplay_data)
{
	if (!adcplay_data) return false;
	if (adcplay_data->buf) adcplay_stop_dma(adcplay_data);
	adcplay_data->buf  = (uint8_t *)malloc(ADCPLAY_DMA_BUF_SIZE * 4);
	if (!adcplay_data->buf) return false;
	adcplay_data->dma_buf[0] = (uint16_t *)adcplay_data->buf;
	adcplay_data->dma_buf[1] = (uint16_t *)(adcplay_data->buf + DACPLAY_DMA_BUF_SIZE * 2);
	adcplay_data->index = 0xFF;
	HAL_ADC_Start_DMA(adcplay_data->hadc, (uint32_t *)adcplay_data->buf, ADCPLAY_DMA_BUF_SIZE * 2);
	ulog_fmt("adcplay: start dma(id:%d)"CRLF, adcplay_data->id);
	return true;
}

void adcplay_init(adcplay_data_t * adcplay_data, uint8_t id, ADC_HandleTypeDef * hadc)
{
	adcplay_data->id = id;
	adcplay_data->hadc = hadc;
	adcplay_data->index = 0xFF;
	adcplay_data->buf = NULL;
	adcplay_data->dma_buf[0] = NULL;
	adcplay_data->dma_buf[1] = NULL;
}

#endif
*/
