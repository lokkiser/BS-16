/*
 * uart_console_util.c
 *
 *  Created on: 12 мар. 2024 г.
 *      Author: Rafael Boltachev
 */


#include "uart_console_util.h"
#include "stm_util.h"
#include <string.h>
#include <stdio.h>
#include "str_util.h"


//static char uart_console_echo[2]= {0};


void uart_console_wait(uart_console_data_t * uart_console_data)
{
	HAL_UART_Receive_IT(uart_console_data->huart, (uint8_t *)&uart_console_data->rcv_byte, 1);
}


void uart_console_callback_data(uart_console_data_t * uart_console_data)
{

	if (uart_console_data->pos >= sizeof(uart_console_data->buf)) uart_console_data->pos = 0;
	uart_console_data->buf[uart_console_data->pos] = uart_console_data->rcv_byte;
	uart_console_data->pos++;
	uart_console_wait(uart_console_data);
}


void uart_console_task_data_rcv(uart_console_data_t * uart_console_data)
{
	char * ptr;
	uint16_t i, pos;


	pos = uart_console_data->pos;
	uart_console_data->pos = 0;

	if (uart_console_data->rcv + pos > sizeof(uart_console_data->buf2)) {
		uart_console_data->rcv = 0;
	}

	ptr = &uart_console_data->buf2[uart_console_data->rcv];
	memcpy(ptr, uart_console_data->buf, pos);

	uart_console_data->rcv += pos;



	for (i = 0; i < pos ; ++i) {
		if ((ptr[i] == '\r') || (ptr[i] == '\n')) {
			ptr[i] = '\r';
			ptr[i + 1] = '\n';
#ifdef UART_CONSOLE_ECHO
			HAL_UART_Transmit(uart_console_data->huart, (uint8_t *)ptr, pos + 1,  10);
#endif
			ptr[i] = '\0';
			uart_console_data->rcv = 0;
			if (uart_console_data->rcv_func) uart_console_data->rcv_func(uart_console_data->buf2);
			return;
		}
	}

#ifdef UART_CONSOLE_ECHO
	HAL_UART_Transmit(uart_console_data->huart, (uint8_t *)ptr, pos,  10);
#endif

}



void uart_console_task_data(uart_console_data_t * uart_console_data)
{
	if (uart_console_data->pos) {
		uart_console_task_data_rcv(uart_console_data);
	}
}

