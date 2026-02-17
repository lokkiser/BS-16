/*
 * uart_tx_util.c
 *
 *  Created on: 13 мар. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "uart_tx_util.h"
#include <string.h>
#include "stm_util.h"


void uart_snd_timeout(UART_HandleTypeDef *huart, uint8_t * data, uint16_t data_len, uint32_t timeout, uart_snd_timeout_finish_t * finish, uart_snd_timeout_data_t * uart_snd_timeout_data)
{
	HAL_UART_Transmit(huart, data, data_len, 1000); //время выполнения примерно 2 млс
	uart_snd_timeout_data->timeout = timeout;
	uart_snd_timeout_data->finish_func = finish;
	uart_snd_timeout_data->tick = HAL_GetTickNoZero();
}

void uart_snd_timeout_abort(uart_snd_timeout_data_t * uart_snd_timeout_data)
{
	uart_snd_timeout_data->tick = 0;
	if (uart_snd_timeout_data->finish_func) uart_snd_timeout_data->finish_func(true);
}

void uart_snd_timeout_task(uart_snd_timeout_data_t * uart_snd_timeout_data)
{
	if (uart_snd_timeout_data->tick) {

		if (HAL_DiffTick(uart_snd_timeout_data->tick) > uart_snd_timeout_data->timeout) {
			uart_snd_timeout_data->tick = 0;
			if (uart_snd_timeout_data->finish_func) uart_snd_timeout_data->finish_func(false);
		}
	}

}



/*

void uart_tx_period_snd_stop(uart_tx_period_snd_data_t * uart_tx_period_snd_data)
{
	uart_tx_period_snd_data->cnt = 0;
}


void uart_tx_period_snd_transmit(uart_tx_period_snd_data_t * uart_tx_period_snd_data)
{
	HAL_UART_Transmit(uart_tx_period_snd_data->huart, uart_tx_period_snd_data->data, uart_tx_period_snd_data->data_len, 1000);
	uart_tx_period_snd_data->cnt--;
	uart_tx_period_snd_data->tick = HAL_GetTick();
}

void uart_tx_period_snd_start(uart_tx_period_snd_data_t * uart_tx_period_snd_data, uint8_t * data, uint16_t data_len, uint32_t period, uint8_t cnt)
{
	uart_tx_period_snd_stop(uart_tx_period_snd_data);

	if (!cnt) return;

	uart_tx_period_snd_data->data_len = MIN(data_len, sizeof(uart_tx_period_snd_data->data));
	memcpy(uart_tx_period_snd_data->data, data, uart_tx_period_snd_data->data_len);
	uart_tx_period_snd_data->period = period;
	uart_tx_period_snd_data->cnt = cnt;

	uart_tx_period_snd_transmit(uart_tx_period_snd_data);
}


void uart_tx_period_snd_task(uart_tx_period_snd_data_t * uart_tx_period_snd_data)
{
	if (!uart_tx_period_snd_data->cnt) return;

	if (HAL_DiffTick(uart_tx_period_snd_data->tick) < uart_tx_period_snd_data->period) return;

	uart_tx_period_snd_transmit(uart_tx_period_snd_data);
}
*/
