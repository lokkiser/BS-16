/*
 * dac_play_util.h
 *
 *  Created on: 19 февр. 2024 г.
 *      Author: Rafael Boltachev
 *
 *      Библотека для воспроизведения звука на DAC через DMA (устарела)
 */

#ifndef DACPLAYBUF_UTIL_H_
#define DACPLAYBUF_UTIL_H_

#include "main.h"
#include "def_util.h"
#include "lwrb.h"

#ifndef DACPLAYBUF_DMA_BUF_SIZE
#define DACPLAYBUF_DMA_BUF_SIZE	640
#endif

#ifndef DACPLAYBUF_LWRB_DATA_SIZE
#define DACPLAYBUF_LWRB_DATA_SIZE	12800
#endif


typedef struct _dacplaybuf_data {

	uint8_t id;
	DAC_HandleTypeDef * hdac;
	uint8_t dac_channel;
	TIM_HandleTypeDef * htim;
	uint32_t frequency;

	uint16_t dma_buf[2][DACPLAYBUF_DMA_BUF_SIZE];

	uint8_t dma_index;
	bool dma_started;

	uint8_t lwrb_data[DACPLAYBUF_LWRB_DATA_SIZE];
	lwrb_t lwrb;
	uint32_t lwrb_write_tick;

} dacplaybuf_data_t;




void dacplaybuf_HalfCpltCallback(dacplaybuf_data_t * dacplaybuf_data); //*
void dacplaybuf_FullCpltCallback(dacplaybuf_data_t * dacplaybuf_data); //*
void dacplaybuf_set_frequency(dacplaybuf_data_t * dacplaybuf_data, uint32_t Frequency); //*
void dacplaybuf_put(dacplaybuf_data_t * dacplaybuf_data, uint16_t * dac_data, uint32_t dac_data_len); //*
size_t dacplaybuf_get_free(dacplaybuf_data_t * dacplaybuf_data); //*
size_t dacplaybuf_get_full(dacplaybuf_data_t * dacplaybuf_data); //*
void dacplaybuf_reset(dacplaybuf_data_t * dacplaybuf_data); //*
void dacplaybuf_task(dacplaybuf_data_t * dacplaybuf_data); //*
void dacplaybuf_init(dacplaybuf_data_t * dacplaybuf_data, uint8_t id, DAC_HandleTypeDef * hdac, uint8_t dac_channel, TIM_HandleTypeDef * htim);
void dacplaybuf_finish(dacplaybuf_data_t * dacplaybuf_data);

uint32_t dacplaybuf_get_frequency(dacplaybuf_data_t * dacplaybuf_data);

lwrb_t * dacplaybuf_get_lwrb(dacplaybuf_data_t * dacplaybuf_data);
void dacplaybuf_stop_dma(dacplaybuf_data_t * dacplaybuf_data);
void dacplaybuf_start_dma(dacplaybuf_data_t * dacplaybuf_data);
bool dacplaybuf_is_dma_started(dacplaybuf_data_t * dacplaybuf_data);

#endif /* UTIL_dacplaybuf_UTIL_H_ */
