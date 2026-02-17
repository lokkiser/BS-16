/*
 * uart_tx_util.h
 *
 *  Created on: 13 мар. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_UART_TX_UTIL_H_
#define UTIL_UART_TX_UTIL_H_

#include "main.h"


typedef void uart_snd_timeout_finish_t(bool abort);

typedef struct _uart_snd_timeout_data {
	uint32_t timeout;
	uint32_t tick;
	uart_snd_timeout_finish_t  * finish_func;
} uart_snd_timeout_data_t;


void uart_snd_timeout(UART_HandleTypeDef *huart, uint8_t * data, uint16_t data_len, uint32_t timeout, uart_snd_timeout_finish_t * finish, uart_snd_timeout_data_t * uart_snd_timeout_data);

void uart_snd_timeout_abort(uart_snd_timeout_data_t * uart_snd_timeout_data);

void uart_snd_timeout_task(uart_snd_timeout_data_t * uart_snd_timeout_data);


#endif /* UTIL_UART_TX_UTIL_H_ */
