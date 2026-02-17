/*
 * adcplay_util.h
 *
 *  Created on: 20 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef ADCPLAY_UTIL_H_
#define ADCPLAY_UTIL_H_

#include "main.h"
#include <stdint.h>
#include "def_util.h"

#ifndef	ADCPLAY_DMA_BUF_SIZE
#define ADCPLAY_DMA_BUF_SIZE	640
#endif

//#ifndef ADCPLAY_VERSION_DYNAMIC

typedef struct _adcplay_data {
	uint8_t id;
	ADC_HandleTypeDef* hadc;
	uint8_t index;
	bool dma_start;
	uint16_t dma_buf[2][ADCPLAY_DMA_BUF_SIZE];
} adcplay_data_t;

#define ADCPLAY_STATIC_INIT(id, hadc) { id, hadc, 0xFF, false, {{0}, {0}} };

//#endif




void adcplay_init(adcplay_data_t* adcplay_data, uint8_t id, ADC_HandleTypeDef* hadc);
void adcplay_HalfCpltCallback(adcplay_data_t* adcplay_data);
void adcplay_FullCpltCallback(adcplay_data_t* adcplay_data);
bool adcplay_start_dma(adcplay_data_t* adcplay_data);
void adcplay_stop_dma(adcplay_data_t* adcplay_data);
bool adcplay_is_dma_started(adcplay_data_t* adcplay_data);

#ifdef USE_ADCPLAY_WRITE_DATA
//!!!! need adc_data_len = ADCPLAY_DMA_BUF_SIZE
void adcplay_write_data(adcplay_data_t* adcplay_data, uint16_t* adc_data);
#endif


/*
#ifdef ADCPLAY_VERSION_DYNAMIC

typedef struct _adcplay_data {
	uint8_t id;
	ADC_HandleTypeDef * hadc;
	uint8_t index;
	uint8_t * buf;
	uint16_t * dma_buf[2];
} adcplay_data_t;

#define ADCPLAY_STATIC_INIT(id, hadc) { id, hadc, 0xFF, NULL, {NULL} };


#endif
*/

#endif /* UTIL_ADCPLAY_UTIL_H_ */
