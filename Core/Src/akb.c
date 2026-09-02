/*
 * akb.c
 *
 *  Created on: 26 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#include "akb.h"
#include "stm_util.h"

static battery_state_e battery_state;
volatile float voltage_bat = 0;
volatile uint8_t en_bat = 1;
//VBAT, Напряжение на АКБ. Отслеживать напряжение АКБ.
static float get_bat_voltage_bat(void)
{
    return (float)get_injected_adc_value(&hadc2, 1, 100) / 4095.0f * 3.3f * 11.0f; // 11 коэффициент делителя
}

void akb_task(void)
{
    voltage_bat = get_bat_voltage_bat();

    if(voltage_bat > 22.0f)
        battery_state = BATTERY_CHARGED; 
    else if (voltage_bat > 20.0f)
        battery_state = BATTERY_LOOSE;
    else
        battery_state = NO_BATTERY;

    if(akb_is_charged())
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

bool akb_is_charged(void)
{
	return 0;
    //return HAL_GPIO_ReadPin(CHRG_OFF_GPIO_Port, CHRG_OFF_Pin);
}

bool is_220_volts(void)
{
    return HAL_GPIO_ReadPin(P220_LED_GPIO_Port, P220_LED_Pin);
}

battery_state_e get_akb_state(void)
{
    return battery_state;
}

void bat_con()
{
	HAL_GPIO_WritePin(CHRG_OFF_GPIO_Port, CHRG_OFF_Pin, en_bat);

}
