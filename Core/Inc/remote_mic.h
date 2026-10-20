/*
 * remote_mic.h
 *
 *  Created on: Sep 9, 2026
 *      Author: bimpk
 */
#include "local_mic.h"
#include "dynbuf_util.h"
#include "local_spk.h"
#ifndef INC_REMOTE_MIC_H_
#define INC_REMOTE_MIC_H_

#define REMOTE_MIC_ADC_HANDLE		&hadc5
#define REMOTE_MIC_ADC_TIM_HANDLE	&htim15

//#define LOCAL_MIC_ADC_TIM_HANDLE	&htim4
void local_mic_dac_HalfCpltCallback(void);
void remote_mic_dac_FullCpltCallback(void);
void remote_mic_adc_HalfCpltCallback(void);
void remote_mic_adc_FullCpltCallback(void);

bool remote_mic_is_started(void);
void remote_mic_stop(void);
bool remote_mic_start(void);
void remote_mic_init(void);
#endif /* INC_REMOTE_MIC_H_ */


extern dynamic_buf_t local_mic_dynbuf;

extern uint16_t remote_mic_dma_adc_buf[2][SOUND_DMA_BUFFER_SIZE];
extern uint16_t remote_mic_dma_dac_buf[2][SOUND_DMA_BUFFER_SIZE];
extern uint8_t  remote_mic_dma_dac_index;
extern bool remote_mic_is_start;
