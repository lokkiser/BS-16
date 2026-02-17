/*
 * local_spk.h
 *
 *  Created on: 19 янв. 2026 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_LOCAL_SPK_H_
#define CORE_INC_LOCAL_SPK_H_

#include "main.h"
#include "dacplay_util.h"
#include "adcplay_util.h"

#define SOUND_BUF_SIZE		1024 * 4

extern dacplay_data_t spk_dacplay_data;
extern adcplay_data_t spk_adcplay_data;

extern  uint8_t sound_buf[SOUND_BUF_SIZE];


#define LOCAL_SPK_ADC_HANDLE		&hadc2
//#define LOCAL_SPK_ADC_TIM_HANDLE	&htim15


#define LOCAL_SPK_DAC_HANDLE		&hdac2
#define LOCAL_SPK_DAC_CHANNEL		DAC_CHANNEL_1
//#define LOCAL_SPK_DAC_TIM_HANDLE	&htim7


void local_spk_adc_HalfCpltCallback(void);
void local_spk_adc_FullCpltCallback(void);
bool local_spk_is_started(void);
void local_spk_stop(void);
bool local_spk_start(void);
void local_spk_dac_HalfCpltCallback(void);
void local_spk_dac_FullCpltCallback(void);

#endif /* CORE_INC_LOCAL_SPK_H_ */
