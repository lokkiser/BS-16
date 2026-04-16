/*
 * ctrl.h
 *
 *  Created on: 29 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_CTRL_H_
#define CORE_INC_CTRL_H_

#include "main.h"
#include "mux.h"

#define LINE_NUM_1    1
#define LINE_NUM_2    2
#define LINE_NUM_3    3
#define LINE_NUM_4    4
#define LINE_NUM_5    5
#define LINE_NUM_6    6
#define LINE_NUM_7    7
#define LINE_NUM_8    8
#define LINE_NUM_9    9
#define LINE_NUM_10  10
#define LINE_NUM_11  11
#define LINE_NUM_12  12
#define LINE_NUM_13  13
#define LINE_NUM_14  14
#define LINE_NUM_15  15
#define LINE_NUM_16  16

typedef enum
{
    NO_EVENT = 0,
    EVENT_PANEL_CALL,
    EVENT_AWAIT_LINE,
    EVENT_LINE_ON,
    EVENT_MISSED_CALL,
} line_event_e;

typedef enum 
{
    LINE_STATE_BREAK    = 0,  //обрыв
	LINE_STATE_DETECTED, 	  //ВП найдено
	LINE_STATE_BUTTON_PRESS,  //ВП нажата кнопка
	LINE_STATE_SHORT_CIRCUIT  //КЗ
} adc_state_e;

#pragma pack(1)
typedef struct
{
    timer_t  timer;
    
    adc_state_e   adc_state;
    line_event_e last_event;
    p_line_state_e p_state;

    uint8_t num;
    uint8_t color_id;

    bool isConfigured; 
} Obj_line_t;
#pragma pack()


adc_state_e get_line_adc_state(uint8_t num);

p_line_state_e get_line_p_state(uint8_t num);

void set_line_event(uint8_t num, line_event_e event);
line_event_e get_last_line_event(uint8_t num);

bool is_line_configurated(uint8_t num);

// extern float ctrl_out_adc[16];
// extern float ctrl_p_out[16];

void ctrl_task(void);
void ctrl_init(void);

void ctrl_FullCpltCallback(ADC_HandleTypeDef* hadc);

#endif /* CORE_INC_CTRL_H_ */
