/*
 * mux.c
 *
 *  Created on: 17 дек. 2025 г.
 *      Author: Rafael Boltachev
 */


#include "mux.h"
#include "pca9535.h"
#include "fault.h"
#include "math_util.h"
#include "mki.h"
#include "ctrl.h"

static pca9535_t line_dd3, line_dd8;
static uint16_t dd3_value = 0, dd8_value = 0;

void set_adc_line(uint8_t num)
{
	PIN_WRITE(MCU_MUX_CTRL_1, (num & 0x01) ? 1 : 0);
	PIN_WRITE(MCU_MUX_CTRL_2, (num & 0x02) ? 1 : 0);
	PIN_WRITE(MCU_MUX_CTRL_3, (num & 0x04) ? 1 : 0);

	if(num < 8)
	{
		PIN_CLR(MCU_MUX_CTRL_5);
		PIN_SET(MCU_MUX_CTRL_4);
	}
	else if(num < 16)
	{
		PIN_CLR(MCU_MUX_CTRL_4);
		PIN_SET(MCU_MUX_CTRL_5);
	}
	else
	{
		PIN_CLR(MCU_MUX_CTRL_4);
		PIN_CLR(MCU_MUX_CTRL_5);
	}
} 

// -------------------------- ON_P_MIC ON_P_SPK --------------------------


//установить на мс DD3 код value
void set_dd3(uint16_t value)
{
	pca9535_set_reg(&line_dd3, PCA9535_REG_TYPE_OUTPUT, PCA9535_PORT_BOTH, (uint8_t*)&value, 2);
	dd3_value = value;
}

//установить на мс DD8 код value
void set_dd8(uint16_t value)
{
	pca9535_set_reg(&line_dd8, PCA9535_REG_TYPE_OUTPUT, PCA9535_PORT_BOTH, (uint8_t*)&value, 2);
	dd8_value = value;
}

//включить/выключить линию on_p = 1...16
void set_on_p(uint8_t on_p, i2c_mux_line_state_e state)
{
	switch (on_p) 
	{
		case 1:
			bitset(&dd3_value, ON_P_1, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_1, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 2:
			bitset(&dd3_value, ON_P_2, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_2, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 3:
			bitset(&dd3_value, ON_P_3, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_3, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 4:
			bitset(&dd3_value, ON_P_4, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_4, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 5:
			bitset(&dd3_value, ON_P_5, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_5, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 6:
			bitset(&dd3_value, ON_P_6, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_6, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 7:
			bitset(&dd3_value, ON_P_7, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_7, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 8:
			bitset(&dd3_value, ON_P_8, 		 !!(state&0x1));
			bitset(&dd3_value, AWAIT_CTRL_8, !!(state&0x2));
			set_dd3(dd3_value);
		break;
		case 9:
			bitset(&dd8_value, ON_P_9, 		 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_9, !!(state&0x2));
			set_dd8(dd8_value);
		break;
		case 10:
			bitset(&dd8_value, ON_P_10, 	 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_10,!!(state&0x2));
			set_dd8(dd8_value);
		break;
		case 11:
			bitset(&dd8_value, ON_P_11, 	 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_11,!!(state&0x2));
			set_dd8(dd8_value);
		break;
		case 12:
			bitset(&dd8_value, ON_P_12, 	 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_12,!!(state&0x2));
			set_dd8(dd8_value);
		break;
		case 13:
			bitset(&dd8_value, ON_P_13, 	 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_13,!!(state&0x2));
			set_dd8(dd8_value);
		break;
		case 14:
			bitset(&dd8_value, ON_P_14, 	 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_14,!!(state&0x2));
			set_dd8(dd8_value);
		break;
		case 15:
			bitset(&dd8_value, ON_P_15, 	 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_15,!!(state&0x2));
			set_dd8(dd8_value);
		break;
		case 16:
			bitset(&dd8_value, ON_P_16, 	 !!(state&0x1));
			bitset(&dd8_value, AWAIT_CTRL_16,!!(state&0x2));
			set_dd8(dd8_value);
		break;
	}

	line_state_e line_state;
	uint8_t color;

	line_state = get_ctrl_out_adc_line_state(on_p);
	color = line_state_to_color(line_state);

	mki_set_led_color(on_p, on_p ? MKI_LED_COLOR_BLUE : color);
}

i2c_mux_line_state_e get_on_p(uint8_t on_p)
{
    uint16_t res = (on_p <= 8) ? dd3_value : dd8_value;
    res >>= ((on_p-1)%8)*2;

    return res&0x3;
}

void mux_init(void)
{
	pca9535_init(&line_dd3, MUX_I2C_HANDLE, MUX_DD3_ADDERESS);
	if (pca9535_is_ready(&line_dd3) != 0) 
	{
		ulog("mux: error DD3 not ready"CRLF);
		set_fault(FAULT_HARDWARE_ERROR);
	}

	pca9535_set_reg(&line_dd3, PCA9535_REG_TYPE_CONFIG, PCA9535_PORT_0, 0, 1); //output
	HAL_Delay(1);
	pca9535_set_reg(&line_dd3, PCA9535_REG_TYPE_CONFIG, PCA9535_PORT_1, 0, 1); //output
	HAL_Delay(1);
	set_dd3(0);
	HAL_Delay(1);

	pca9535_init(&line_dd8, MUX_I2C_HANDLE, MUX_DD8_ADDERESS);
	if (pca9535_is_ready(&line_dd8) != 0) 
	{
		ulog("mux: error DD4 not ready"CRLF);
		set_fault(FAULT_HARDWARE_ERROR);
	}
	pca9535_set_reg(&line_dd8, PCA9535_REG_TYPE_CONFIG, PCA9535_PORT_0, 0, 1); //output
	HAL_Delay(1);
	pca9535_set_reg(&line_dd8, PCA9535_REG_TYPE_CONFIG, PCA9535_PORT_1, 0, 1); //output
	HAL_Delay(1);
	set_dd8(0);
	HAL_Delay(1);
}

bool mux_is_active_line(void)
{
	return ((dd3_value & 0x5555) || (dd8_value & 0x5555));
}

void mux_cancel_all(void)
{
	set_dd3(dd3_value & ~0x5555);
	set_dd8(dd8_value & ~0x5555);
}

/*
//микросхемы dd3 и dd8
//установить на мс DD3 и DD8 в ON_P - 0 или 1
void set_on_p(uint8_t on_p, uint8_t value)
{
	if (value) SETBIT(dd3_dd8_value, on_p);
	else CLRBIT(dd3_dd8_value, on_p);


	if (on_p > AWAIT_CTRL_8) set_dd8(dd3_dd8_value >> 16);
	else set_dd3(dd3_dd8_value & 0xFFFF);
}

uint8_t get_on_p(uint8_t on_p)
{
	return GETBIT(dd3_dd8_value, on_p);
}
*/
