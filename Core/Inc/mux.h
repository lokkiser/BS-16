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

typedef enum
{
    P_IDLE  = 0x0,
    P_ON    = 0x1,
    P_AWAIT = 0x2,
    P_ERROR = 0x3,
} i2c_mux_line_state_e;


#define OUT_ADC1	0
#define OUT_ADC2	1
#define OUT_ADC3	2
#define OUT_ADC4	3
#define OUT_ADC5	4
#define OUT_ADC6	5
#define OUT_ADC7	6
#define OUT_ADC8	7
// void set_dd2(uint8_t value);  // DD2 -> DD2

#define OUT_ADC9	0
#define OUT_ADC10	1
#define OUT_ADC11	2
#define OUT_ADC12	3
#define OUT_ADC13	4
#define OUT_ADC14	5
#define OUT_ADC15	6
#define OUT_ADC16	7
// void set_dd4(uint8_t value);  // DD5 -> DD4


//void set_out_adc(uint8_t out_adc1_adc16);

#define P_OUT_1		0
#define P_OUT_2		1
#define P_OUT_3		2
#define P_OUT_4		3
#define P_OUT_5		4
#define P_OUT_6		5
#define P_OUT_7		6
#define P_OUT_8		7
// void set_dd1(uint8_t value);   // DD1 -> DD1

#define P_OUT_9		0
#define P_OUT_10	1
#define P_OUT_11	2
#define P_OUT_12	3
#define P_OUT_13	4
#define P_OUT_14	5
#define P_OUT_15	6
#define P_OUT_16	7
// void set_dd5(uint8_t value);   // DD6-> DD5


//void set_p_out(uint8_t p_out_1_p_out_16);


#define MUX_I2C_HANDLE		&hi2c3
#define MUX_DD3_ADDERESS	(0x21)     // DD3 -> DD3
#define MUX_DD8_ADDERESS	(0x20)     // DD4 -> DD8


#define ON_P_1        0
#define AWAIT_CTRL_1  1
#define ON_P_2        2
#define AWAIT_CTRL_2  3
#define ON_P_3        4
#define AWAIT_CTRL_3  5
#define ON_P_4        6
#define AWAIT_CTRL_4  7
#define ON_P_5        8
#define AWAIT_CTRL_5  9
#define ON_P_6        10
#define AWAIT_CTRL_6  11
#define ON_P_7        12
#define AWAIT_CTRL_7  13
#define ON_P_8        14
#define AWAIT_CTRL_8  15
// void set_dd3(uint16_t value);  // DD3 -> DD3

#define ON_P_9        0
#define AWAIT_CTRL_9  1
#define ON_P_10       2
#define AWAIT_CTRL_10 3
#define ON_P_11       4
#define AWAIT_CTRL_11 5
#define ON_P_12       6
#define AWAIT_CTRL_12 7
#define ON_P_13       8
#define AWAIT_CTRL_13 9
#define ON_P_14       10
#define AWAIT_CTRL_14 11
#define ON_P_15       12
#define AWAIT_CTRL_15 13
#define ON_P_16       14
#define AWAIT_CTRL_16 15
// void set_dd8(uint16_t value);  // DD4 -> DD8

void set_adc_line(uint8_t num);
void set_on_p(uint8_t on_p, i2c_mux_line_state_e value);
i2c_mux_line_state_e get_on_p(uint8_t on_p);
void mux_init(void);
bool mux_is_active_line(void);

#endif /* CORE_INC_MUX_H_ */
