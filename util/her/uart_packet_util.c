/*
 * uart_packet_util.c
 *
 *  Created on: 3 апр. 2024 г.
 *      Author: Rafael Boltachev
 */
#include "uart_packet_util.h"
#include "stm_util.h"
#include <string.h>



void uart_packet_wait(uart_packet_data_t * uart_packet_data)
{
	HAL_UART_Receive_IT(uart_packet_data->huart, &uart_packet_data->rcv_byte, 1);
}

void uart_packet_callback_data(uart_packet_data_t * uart_packet_data)
{
	if (uart_packet_data->pos >= sizeof(uart_packet_data->buf)) uart_packet_data->pos = 0;
	uart_packet_data->buf[uart_packet_data->pos] = uart_packet_data->rcv_byte;
	uart_packet_data->pos++;
	uart_packet_data->tick = HAL_GetTickNoZero();
	uart_packet_wait(uart_packet_data);
}

void uart_packet_task_data(uart_packet_data_t * uart_packet_data)
{
	if (uart_packet_data->tick) {
		if ( HAL_DiffTick(uart_packet_data->tick) > UART_PACKET_TIMEOUT) {
			ulog_fmt("urcv %d", uart_packet_data->pos);
			 uart_packet_data->tick = 0;
			 if (uart_packet_data->pos > 0) {
				 uint16_t pos = uart_packet_data->pos;
				 uart_packet_data->pos = 0;
				 memcpy(uart_packet_data->buf2, uart_packet_data->buf, pos);
				 uart_packet_data->rcv_func(uart_packet_data->buf2, pos);
			 }
		}
	}
}


void uart_packet_init(uart_packet_data_t * uart_packet_data, UART_HandleTypeDef *huart, uart_packet_rcv_func_t * rcv_func)
{
	uart_packet_data->huart = huart;
	uart_packet_data->rcv_func = rcv_func;
	uart_packet_data->pos = 0;
	uart_packet_data->tick = 0;
	uart_packet_wait(uart_packet_data);
}


