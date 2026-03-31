/*
 * ctrl.h
 *
 *  Created on: 29 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_CTRL_H_
#define CORE_INC_CTRL_H_

#include "main.h"

typedef enum 
{
    LINE_STATE_BREAK = 0, 	 //обрыв
	LINE_STATE_DETECTED, 	 //ВП найдено
	LINE_STATE_BUTTON_PRESS, //ВП нажата кнопка
	LINE_STATE_SHORT_CIRCUIT //КЗ
} adc_state_e;

adc_state_e get_ctrl_out_adc_line_state(uint8_t number);
uint8_t line_state_to_color(adc_state_e adc_state);


extern float ctrl_out_adc[16];
extern float ctrl_p_out[16];

void ctrl_task(void);
void ctrl_init(void);

void ctrl_FullCpltCallback(ADC_HandleTypeDef* hadc);

#endif /* CORE_INC_CTRL_H_ */
