/*
 * akb.c
 *
 *  Created on: 26 дек. 2025 г.
 *      Author: Rafael Boltachev
 */


#include "akb.h"
#include "stm_util.h"


//VBAT, Напряжение на АКБ. Отслеживать напряжение АКБ.
float get_bat_voltage(void)
{
    return get_injected_adc_value(&hadc2, 2, 100); //PA7 ADC2_IN4 RANk 2
    // return get_adc_value(&hadc2, 100);
}

void akb_task(void)
{
    if ((get_bat_voltage() / 4095.0f * 3.3f) < 2.0) // TODO
    {
    } 
    else // BAT_CHARGED - вход указывает на то, есть ли ток через АКБ (если 3.3В, то ток не течёт).
    {
    }
}

void akb_init(void)
{
    HAL_ADC_Start(&hadc2);
}
