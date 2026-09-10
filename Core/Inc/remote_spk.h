/*
 * remote_spk.h
 *
 *  Created on: 19 янв. 2026 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_remote_SPK_H_
#define CORE_INC_remote_SPK_H_


#include "main.h"
#include "sai.h"
#define TOTAL_SAMPLES 2048
#define TDM_CHANNELS  8 // Например, стандартный TDM-8 frame
#define AUDIO_BUF_SIZE 4096
#define remote_SPK_ADC_HANDLE		&hadc5
#define remote_SPK_ADC_TIM_HANDLE	&htim15

#define remote_SPK_SAI_HANDLE		hsai_BlockA1
//#include "dacplay_util.h"
//#include "adcplay_util.h"

//extern dacplay_data_t spk_dacplay_data;
//extern adcplay_data_t spk_adcplay_data;

extern uint32_t sai_buffer[TOTAL_SAMPLES * TDM_CHANNELS];


//#define remote_SPK_DAC_TIM_HANDLE	&htim7


bool remote_spk_is_started(void);
void remote_spk_stop(void);
bool remote_spk_start(void);


#endif /* CORE_INC_remote_SPK_H_ */
