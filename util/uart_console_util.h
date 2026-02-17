/*
 * uart_console_util.h
 *
 *  Created on: 12 мар. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_UART_CONSOLE_UTIL_H_
#define UTIL_UART_CONSOLE_UTIL_H_

#include "main.h"

#ifndef	UART_CONSOLE_BUF_SIZE
#define UART_CONSOLE_BUF_SIZE	128
#endif

typedef void uart_console_rcv_func_t(char * msg);

typedef struct  _uart_console_data {
	UART_HandleTypeDef *huart;
	uart_console_rcv_func_t * rcv_func;
	char rcv_byte;
	uint16_t pos;
	char buf[UART_CONSOLE_BUF_SIZE];
	uint16_t rcv;
	char buf2[UART_CONSOLE_BUF_SIZE];
} uart_console_data_t;





void uart_console_task_data(uart_console_data_t * uart_console_data);
void uart_console_callback_data(uart_console_data_t * uart_console_data);
void uart_console_wait(uart_console_data_t * uart_console_data);



#endif /* UTIL_UART_CONSOLE_UTIL_H_ */
