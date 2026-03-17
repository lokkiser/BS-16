/*
 * ctrl.c
 *
 *  Created on: 29 дек. 2025 г.
 *      Author: Rafael Boltachev
 */


#include "ctrl.h"
#include "mux.h"
#include "stm_util.h"
#include "fault.h"
#include "mki.h"

//Контроль выходов АУ, их микрофона и динамика
//Мы переключаем через мультиплексор каналы и через делитель смотрим,
//какое напряжение мы получаем на делителе.
//Если там 3.3, то обрыв, 0 кз и в таком духе.

#define CTRL_VOLTAGE_OBRIV_OUT_ADC	3.1f
#define CTRL_VOLTAGE_OBRIV_P_OUT	2.6f
#define CTRL_VOLTAGE_NORMA			1.65f
#define CTRL_VOLTAGE_KZ				0.2f

static uint8_t ctrl_index = 0;
float ctrl_out_adc_old[16];
float ctrl_out_adc[16];
float ctrl_p_out[16];

uint16_t adc_sound_buf[1];


line_state_e voltage_out_adc_to_line_state(float volt)
{
	if 		(volt > CTRL_VOLTAGE_OBRIV_OUT_ADC) return LINE_STATE_BREAK;
	else if (volt > CTRL_VOLTAGE_OBRIV_P_OUT) 	return LINE_STATE_DETECTED;
	else if (volt > CTRL_VOLTAGE_KZ) 			return LINE_STATE_BUTTON_PRESS;
	else 										return LINE_STATE_SHORT_CIRCUIT;
}

uint8_t line_state_to_color(line_state_e line_state)
{
	switch (line_state) 
	{
		case LINE_STATE_BREAK: 		   return MKI_LED_COLOR_BLACK;
		case LINE_STATE_DETECTED: 	   return MKI_LED_COLOR_GREEN;
		case LINE_STATE_BUTTON_PRESS:  return MKI_LED_COLOR_LIGHTBLUE;
		case LINE_STATE_SHORT_CIRCUIT: return MKI_LED_COLOR_YELLOW;
		default: 					   return MKI_LED_COLOR_BLACK;
	}
}

line_state_e get_ctrl_out_adc_line_state(uint8_t number)
{
	return voltage_out_adc_to_line_state(ctrl_out_adc[number]);
}

float ctrl_adc_to_voltage(uint16_t value)
{
	return 3.3f * (float)value / 4095.0f;
}

//EXT_SPK_I
//Контролируем ток (средняя точка и от неё выше-нише) через усилитель (амплитуда) можем вычитывать через АЦП.
float get_ext_spk_i(void)
{
	return ctrl_adc_to_voltage(get_injected_adc_value(&hadc5, 1, 100)); 
}

//EXT_SPK_OFFSET
//задаёт ток сдвига. Это надо чтобы на выход подавать какое-то напряжение, достаточное для питания светодиода.
//Т.е. мы подаём звук (переменная составляющая) + постоянная составляющая.
void set_ext_spk_offset(float volt)
{
	uint32_t value = 4095.0f * volt / 3.3f;

	HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_2, DAC_ALIGN_12B_R, value);
}

static uint16_t ctrl_adc1_dma[2] = {0};
static uint16_t ctrl_adc2_dma[1] = {0};
// static uint16_t ctrl_adc3_dma[2] = {0};

void ctrl_FullCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc == &hadc1) 
	{
		ctrl_out_adc_old[ctrl_index] = ctrl_out_adc[ctrl_index];
		ctrl_out_adc[ctrl_index] = ctrl_adc_to_voltage(ctrl_adc1_dma[0]);

		adc_sound_buf[0] = (mux_is_active_line()) ? ctrl_adc1_dma[1] : 0;
		// ctrl_out_adc_old[8 + ctrl_index] = ctrl_out_adc[8  + ctrl_index];
		// ctrl_out_adc[8 + ctrl_index] = ctrl_adc_to_voltage(ctrl_adc1_dma[1]);

		//fulog_fmt("a1.1:%d a1.3:%d"CRLF, ctrl_adc1_dma[0], ctrl_adc1_dma[1]);
	}
	else if (hadc == &hadc2) 
	{
		ctrl_p_out[ctrl_index] = ctrl_adc_to_voltage(ctrl_adc2_dma[0]);
		// ctrl_p_out[8 + ctrl_index] = ctrl_adc_to_voltage(ctrl_adc3_dma[1]);
		
		//fulog_fmt("a3.2:%d a3.6:%d"CRLF, ctrl_adc3_dma[0], ctrl_adc3_dma[1]);
	}
}

void ctrl_button_press(uint8_t index)
{
	ulog_fmt("ctrl: VP button press %d"CRLF, index);
}

/*
void ctrl_button_unpress(uint8_t index)
{
}
*/

void ctrl_task(void)
{
	line_state_e line_state;
	uint8_t color;

	line_state = get_ctrl_out_adc_line_state(ctrl_index);
	color = line_state_to_color(line_state);
	if (mki_get_led_color(ctrl_index) != color) 
		mki_set_led_color(ctrl_index, 	 color);

	if (line_state == LINE_STATE_BUTTON_PRESS) 
	{
		if (voltage_out_adc_to_line_state(ctrl_out_adc_old[ctrl_index]) != LINE_STATE_BUTTON_PRESS) 
		{
			ctrl_button_press(ctrl_index);
		}
	}
	/*
	else if (line_state != LINE_STATE_BUTTON_PRESS) 
	{
		if (voltage_out_adc_to_line_state(ctrl_out_adc_old[ctrl_index]) == LINE_STATE_BUTTON_PRESS) 
	 	{
			ctrl_button_unpress(ctrl_index);
		}
	}
	*/

	/*
	line_state = get_ctrl_out_adc_line_state(8 + ctrl_index);
	color = line_state_to_color(line_state);
	if (mki_get_led_color(8 + ctrl_index) != color) 
		mki_set_led_color(8 + ctrl_index, 	 color);
	*/

	/*
	ctrl_out_adc[ctrl_index] = ctrl_adc_to_voltage(get_injected_adc_value(&hadc1, ADC_INJECTED_RANK_1, 10));
	ctrl_out_adc[8 + ctrl_index] = ctrl_adc_to_voltage(get_injected_adc_value(&hadc1, ADC_INJECTED_RANK_2, 10));
	ctrl_p_out[ctrl_index] = ctrl_adc_to_voltage(get_injected_adc_value(&hadc3, ADC_INJECTED_RANK_1, 10));
	ctrl_p_out[8 + ctrl_index] = ctrl_adc_to_voltage(get_injected_adc_value(&hadc3, ADC_INJECTED_RANK_2, 10));

	//OUT_ADC	3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2,3.2
	//P_OUT		2.7,2.7,2.7,2.7,1.6,2.7,2.7,2.7,2.7,2.7,2.7,2.7,2.4,2.7,2.7,2.7
	*/

	if (ctrl_out_adc[ctrl_index] < CTRL_VOLTAGE_KZ) 
		set_fault(FAULT_OUT_ADC1_KZ + ctrl_index);
	else if (ctrl_out_adc[ctrl_index] > CTRL_VOLTAGE_OBRIV_OUT_ADC) 
		set_fault(FAULT_OUT_ADC1_OBRIV + ctrl_index);
	else 
	{
		unset_fault(FAULT_OUT_ADC1_KZ + ctrl_index);
		unset_fault(FAULT_OUT_ADC1_OBRIV + ctrl_index);
	}

	/*
	if (ctrl_out_adc[ctrl_index + 8] < CTRL_VOLTAGE_KZ) 
		set_fault(FAULT_OUT_ADC8_KZ + ctrl_index);
	else if (ctrl_out_adc[ctrl_index + 8] > CTRL_VOLTAGE_OBRIV_OUT_ADC) 
		set_fault(FAULT_OUT_ADC8_OBRIV + ctrl_index);
	else 
	{
		unset_fault(FAULT_OUT_ADC8_KZ + ctrl_index);
		unset_fault(FAULT_OUT_ADC8_OBRIV + ctrl_index);
	}
	*/

	if (ctrl_p_out[ctrl_index] < CTRL_VOLTAGE_KZ) 
		set_fault(FAULT_P_OUT_1_KZ + ctrl_index);
	else if (ctrl_p_out[ctrl_index] > CTRL_VOLTAGE_OBRIV_P_OUT) 
		set_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
	else 
	{
		unset_fault(FAULT_P_OUT_1_KZ + ctrl_index);
		unset_fault(FAULT_P_OUT_1_OBRIV + ctrl_index);
	}

	/*if (ctrl_p_out[ctrl_index + 8] < CTRL_VOLTAGE_KZ) 
		set_fault(FAULT_P_OUT_8_KZ + ctrl_index);
	else if (ctrl_p_out[ctrl_index + 8] > CTRL_VOLTAGE_OBRIV_P_OUT) 
		set_fault(FAULT_P_OUT_8_OBRIV + ctrl_index);
	else 
	{
		unset_fault(FAULT_P_OUT_8_KZ + ctrl_index);
		unset_fault(FAULT_P_OUT_8_OBRIV + ctrl_index);
	}
	*/

	ctrl_index++;
    ctrl_index %= 16;
	
	// ctrl_index = (ctrl_index > 16) ? 1 : ctrl_index;

	set_adc_line(ctrl_index);

	/*	
	if (ctrl_index > 8) 
		ctrl_index = 1;

	set_dd2(ctrl_index - 1);
	set_dd5(ctrl_index - 1);
	set_dd1(ctrl_index - 1);
	set_dd6(ctrl_index - 1);
	*/

	HAL_StatusTypeDef status;
	status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ctrl_adc1_dma, 2);
	if (status != HAL_OK) 
	{
		ulog_fmt("ctrl: error invalid start dma adc1 status = %d"CRLF, status);
	}

	status = HAL_ADC_Start_DMA(&hadc2, (uint32_t*)ctrl_adc2_dma, 1);
	if (status != HAL_OK) 
	{
		ulog_fmt("ctrl: error invalid start dma adc2 status = %d"CRLF, status);
	}
}

void ctrl_init(void)
{
	for (uint8_t i = 0; i < 16; i++) 
	{
		ctrl_out_adc[i] = CTRL_VOLTAGE_NORMA;
		ctrl_p_out[i]   = CTRL_VOLTAGE_NORMA;
	}

	//HAL_ADC_Start(&hadc2);
	HAL_DAC_Start(&hdac2, DAC_CHANNEL_2);
}
