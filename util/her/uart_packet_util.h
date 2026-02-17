/*
 * uart_packet_util.h
 *
 *  Created on: 3 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_UART_PACKET_UTIL_H_
#define UTIL_UART_PACKET_UTIL_H_


#include "main.h"

#define UART_PACKET_BUF_SIZE	128
#define UART_PACKET_TIMEOUT		10

typedef void uart_packet_rcv_func_t(uint8_t * msg, uint16_t msg_size);

typedef struct  _uart_packet_data {
	UART_HandleTypeDef *huart;
	uart_packet_rcv_func_t * rcv_func;
	uint8_t rcv_byte;
	uint16_t pos;
	uint16_t tick;
	uint8_t buf[UART_PACKET_BUF_SIZE];
	uint8_t buf2[UART_PACKET_BUF_SIZE];
} uart_packet_data_t;



void uart_packet_task_data(uart_packet_data_t * uart_packet_data);
void uart_packet_callback_data(uart_packet_data_t * uart_packet_data);
void uart_packet_wait(uart_packet_data_t * uart_packet_data);
void uart_packet_init(uart_packet_data_t * uart_packet_data, UART_HandleTypeDef *huart, uart_packet_rcv_func_t * rcv_func);






#endif /* UTIL_UART_PACKET_UTIL_H_ */
