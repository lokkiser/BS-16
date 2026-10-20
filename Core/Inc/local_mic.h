/*
 * local_mic.h
 *
 *  Created on: 22 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_LOCAL_MIC_H_
#define CORE_INC_LOCAL_MIC_H_

#include "main.h"

#define LOCAL_MIC_ADC_HANDLE		&hadc4
#define LOCAL_MIC_ADC_TIM_HANDLE	&htim20

struct adc_mic_snd
{//буфер микрофона
	//uint16_t snd_buf[200];
	uint16_t index;
};
//#define LOCAL_MIC_DAC_TIM_HANDLE	&htim8
#endif /* CORE_INC_LOCAL_MIC_H_ */
bool local_mic_start(void);


extern struct adc_mic_snd local_mic_adc;
