/*
 * uart_rx_idle_util.h
 *
 *  Created on: 4 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UART_RX_IDLE_H_
#define UART_RX_IDLE_H_


#include "main.h"
#include "dynbuf_util.h"

#ifndef UART_RX_IDLE_BUF_SIZE
#define UART_RX_IDLE_BUF_SIZE	1600
#endif


typedef struct  _uart_rx_idle_data {
	UART_HandleTypeDef *huart;
	uint8_t buf[UART_RX_IDLE_BUF_SIZE];
	dynamic_buf_t dynbuf;
} uart_rx_idle_data_t;


//должно вызываться из основного потока программы
void uart_rx_idle_init(uart_rx_idle_data_t * uart_rx_idle_data, UART_HandleTypeDef *huart, size_t max_alloc_size);
//должно вызываться из void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
void uart_rx_idle_callback_data(uart_rx_idle_data_t * uart_rx_idle_data, uint16_t Size);
//должно вызываться из основного потока программы
void uart_rx_idle_task_data(uart_rx_idle_data_t * uart_rx_idle_data);
//Нужно определить функцию прихода данных с UART
void uart_rx_idle_rcv_data(uart_rx_idle_data_t * uart_rx_idle_data, uint8_t * msg, uint16_t msg_size);

#endif /* UART_RX_IDLE_H_ */
