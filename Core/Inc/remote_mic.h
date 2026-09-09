/*
 * remote_mic.h
 *
 *  Created on: Sep 9, 2026
 *      Author: bimpk
 */
#include "local_mic.h"
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

struct adc_mic_snd
{//буфер микрофона
	uint16_t snd_buf[200];
	uint16_t index;
};
extern struct adc_mic_snd mic_adc;
