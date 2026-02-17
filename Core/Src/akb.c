/*
 * akb.c
 *
 *  Created on: 26 дек. 2025 г.
 *      Author: Rafael Boltachev
 */


#include "akb.h"
#include "stm_util.h"


//DAC2_1 управление током заряда.
void set_tok_zaryada(uint32_t value)
{
	//HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
}

//VBAT, Напряжение на АКБ. Отслеживать напряжение АКБ.
float get_bat_voltage(void)
{
	//return get_injected_adc_value(&hadc2, 2, 100); //PA7 ADC2_IN4 RANk 2
	return get_adc_value(&hadc2, 100);
}


void akb_task(void)
{
	if (get_bat_voltage() < 2.0) 
	{
		//BAT_CHARGED указывает на то, есть ли ток через АКБ (если 3.3В, то ток не течёт).
		PIN_CLR(BAT_CHARGED); //BAT_CHARGED
	} 
	else 
	{
		//BAT_CHARGED указывает на то, есть ли ток через АКБ (если 3.3В, то ток не течёт).
		PIN_SET(BAT_CHARGED); //BAT_CHARGED
	}

	//DAC2_1 управление током заряда
	set_tok_zaryada(4095);

}

void akb_init(void)
{
	HAL_ADC_Start(&hadc2);
	//HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
}
