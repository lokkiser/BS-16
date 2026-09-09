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

#define LOCAL_MIC_ADC_HANDLE		&hadc4
#define LOCAL_MIC_ADC_TIM_HANDLE	&htim20


#define LOCAL_MIC_DAC_HANDLE		&hdac1
#define LOCAL_MIC_DAC_CHANNEL		DAC_CHANNEL_2
//#define LOCAL_MIC_DAC_TIM_HANDLE	&htim8






extern dynamic_buf_t local_mic_dynbuf;

#endif /* CORE_INC_LOCAL_MIC_H_ */

