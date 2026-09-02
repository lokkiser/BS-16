/*
 * mux.h
 *
 *  Created on: 17 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_MUX_H_
#define CORE_INC_MUX_H_

#include "main.h"
#include "stm_util.h"

#define MUX_I2C_HANDLE		&hi2c3
#define MUX_DD3_ADDERESS	(0x46)>>1
#define MUX_DD4_ADDERESS	(0x44)>>1

typedef enum
{
    P_IDLE  = 0x0,
    P_ON    = 0x1,
    P_AWAIT = 0x2,
    P_ERROR = 0x3,
} p_line_state_e;

#pragma pack(1)
typedef union
{
    struct
    {
        uint16_t dd3;
        uint16_t dd4;
    } data;

    struct 
    {
        p_line_state_e p_1   :2;
        p_line_state_e p_2   :2;
        p_line_state_e p_3   :2;
        p_line_state_e p_4   :2;

        p_line_state_e p_5   :2;
        p_line_state_e p_6   :2;
        p_line_state_e p_7   :2;
        p_line_state_e p_8   :2;

        p_line_state_e p_9   :2;
        p_line_state_e p_10  :2;
        p_line_state_e p_11  :2;
        p_line_state_e p_12  :2;

        p_line_state_e p_13  :2;
        p_line_state_e p_14  :2;
        p_line_state_e p_15  :2;
        p_line_state_e p_16  :2;      
    } lines;
} line_p_outs_u;
#pragma pack()

// #define OUT_ADC1	    0
// #define OUT_ADC2	    1
// #define OUT_ADC3	    2
// #define OUT_ADC4	    3
// #define OUT_ADC5	    4
// #define OUT_ADC6	    5
// #define OUT_ADC7	    6
// #define OUT_ADC8	    7
// 
// #define OUT_ADC9	    0
// #define OUT_ADC10	1
// #define OUT_ADC11	2
// #define OUT_ADC12	3
// #define OUT_ADC13	4
// #define OUT_ADC14	5
// #define OUT_ADC15	6
// #define OUT_ADC16	7


//void set_out_adc(uint8_t out_adc1_adc16);

// #define P_OUT_1		0
// #define P_OUT_2		1
// #define P_OUT_3		2
// #define P_OUT_4		3
// #define P_OUT_5		4
// #define P_OUT_6		5
// #define P_OUT_7		6
// #define P_OUT_8		7
// 
// #define P_OUT_9		0
// #define P_OUT_10	    1
// #define P_OUT_11	    2
// #define P_OUT_12	    3
// #define P_OUT_13	    4
// #define P_OUT_14	    5
// #define P_OUT_15	    6
// #define P_OUT_16	    7



// #define ON_P_1        0
// #define AWAIT_CTRL_1  1
// #define ON_P_2        2
// #define AWAIT_CTRL_2  3
// #define ON_P_3        4
// #define AWAIT_CTRL_3  5
// #define ON_P_4        6
// #define AWAIT_CTRL_4  7
// #define ON_P_5        8
// #define AWAIT_CTRL_5  9
// #define ON_P_6        10
// #define AWAIT_CTRL_6  11
// #define ON_P_7        12
// #define AWAIT_CTRL_7  13
// #define ON_P_8        14
// #define AWAIT_CTRL_8  15
// 
// #define ON_P_9        0
// #define AWAIT_CTRL_9  1
// #define ON_P_10       2
// #define AWAIT_CTRL_10 3
// #define ON_P_11       4
// #define AWAIT_CTRL_11 5
// #define ON_P_12       6
// #define AWAIT_CTRL_12 7
// #define ON_P_13       8
// #define AWAIT_CTRL_13 9
// #define ON_P_14       10
// #define AWAIT_CTRL_14 11
// #define ON_P_15       12
// #define AWAIT_CTRL_15 13
// #define ON_P_16       14
// #define AWAIT_CTRL_16 15

void set_mux_line(uint8_t num);
void set_on_p(uint8_t on_p, p_line_state_e value);
p_line_state_e get_on_p(uint8_t on_p);

void mux_init(void);
bool mux_is_active_line(void);
bool mux_is_awaiting_line(void);
bool mux_active_line_num(uint8_t* num);
void mux_cancel_all(void);

void set_dd3(uint16_t value);
void set_dd4(uint16_t value);

#endif /* CORE_INC_MUX_H_ */
