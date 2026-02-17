/*
 * dacplay_util.h
 *
 *  Created on: 20 мая 2024 г.
 *      Author: Rafael Boltachev
 *
 *      Библотека для воспроизведения звука на DAC через DMA
 */

#ifndef DACPLAY_UTIL_H_
#define DACPLAY_UTIL_H_

#include "main.h"
#include "def_util.h"

#ifndef DACPLAY_DMA_BUF_SIZE
#define DACPLAY_DMA_BUF_SIZE	640
#endif

#ifndef DACPLAY_ZERO_VALUE
#define DACPLAY_ZERO_VALUE 2047
#endif

//#ifndef DACPLAY_VERSION_DYNAMIC


typedef struct _dacplay_data 
{
	uint8_t id; //уникальный идентификатор
	uint8_t index; //текущий свободный индекс dma_buf-ера, (0 - свободен 0 буфер, 1 - свободен 1 буфер,  или 0xFF - заняты оба буфера)
	DAC_HandleTypeDef * hdac; //Handle DAC-a
	uint8_t dac_channel; //канал DAC-а
	bool dma_started; //запущено ли DMA
	uint16_t dma_buf[2][DACPLAY_DMA_BUF_SIZE]; //буффер под звук
} dacplay_data_t;

#define DACPLAY_STATIC_INIT(id, hdac, dac_channel) 	{id, 0xFF, hdac, dac_channel, false, {{0}, {0}}}

//#endif






//инициализация
void dacplay_init(dacplay_data_t * dacplay_data, uint8_t id, DAC_HandleTypeDef * hdac, uint8_t dac_channel);
//нобходимо вызывать в HAL_DAC_ConvHalfCpltCallback
void dacplay_HalfCpltCallback(dacplay_data_t * dacplay_data);
//нобходимо вызывать в HAL_DAC_ConvCpltCallback
void dacplay_FullCpltCallback(dacplay_data_t * dacplay_data);
//остановить DMA
void dacplay_stop_dma(dacplay_data_t * dacplay_data);
//запустить DMA
bool dacplay_start_dma(dacplay_data_t * dacplay_data);
//DMA запущено?
bool dacplay_is_dma_started(dacplay_data_t * dacplay_data);

#ifdef USE_DACPLAY_READ_DATA
//!!!! need  dac_data_len = DACPLAY_DMA_BUF_SIZE
void dacplay_read_data(dacplay_data_t * dacplay_data, uint16_t * dac_data);
#endif



/*
#ifdef DACPLAY_VERSION_DYNAMIC

typedef struct _dacplay_data {

	uint8_t id; //уникальный идентификатор
	uint8_t index; //текущий свободный индекс dma_buf-ера, (0 - свободен 0 буфер, 1 - свободен 1 буфер,  или 0xFF - заняты оба буфера)
	DAC_HandleTypeDef * hdac; //Handle DAC-a
	uint8_t dac_channel; //канал DAC-а
	uint8_t * buf; //буффер под звук
	uint16_t * dma_buf[2]; //указатели на буффер под звук
} dacplay_data_t;


#endif
*/

#endif /* DACPLAY_UTIL_H_ */
