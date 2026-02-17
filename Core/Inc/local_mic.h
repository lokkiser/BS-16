/*
 * local_mic.h
 *
 *  Created on: 22 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_LOCAL_MIC_H_
#define CORE_INC_LOCAL_MIC_H_

#include "main.h"
#include "dynbuf_util.h"

#define LOCAL_MIC_ADC_HANDLE		&hadc5
//#define LOCAL_MIC_ADC_TIM_HANDLE	&htim15


#define LOCAL_MIC_DAC_HANDLE		&hdac1
#define LOCAL_MIC_DAC_CHANNEL		DAC_CHANNEL_1
//#define LOCAL_MIC_DAC_TIM_HANDLE	&htim8


void local_mic_adc_HalfCpltCallback(void);
void local_mic_adc_FullCpltCallback(void);
bool local_mic_is_started(void);
void local_mic_stop(void);
bool local_mic_start(void);
void local_mic_init(void);
void local_mic_dac_HalfCpltCallback(void);
void local_mic_dac_FullCpltCallback(void);

extern dynamic_buf_t local_mic_dynbuf;

#endif /* CORE_INC_LOCAL_MIC_H_ */
