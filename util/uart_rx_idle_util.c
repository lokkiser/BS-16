/*
 * uart_rx_idle_util.c
 *
 *  Created on: 4 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

//#define 	UART_IS_IDLE(huart) 	((READ_REG(huart->Instance->SR) & USART_SR_IDLE) != RESET) && ((READ_REG(huart->Instance->SR) & USART_CR1_IDLEIE) != RESET))

#include "uart_rx_idle_util.h"


void uart_rx_idle_wait(uart_rx_idle_data_t * uart_rx_idle_data)
{
	HAL_UARTEx_ReceiveToIdle_IT(uart_rx_idle_data->huart, uart_rx_idle_data->buf, sizeof(uart_rx_idle_data->buf));
}

//должно вызываться из void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
void uart_rx_idle_callback_data(uart_rx_idle_data_t * uart_rx_idle_data, uint16_t Size)
{
	uart_rx_idle_data->dynbuf.busy = TRUE;
	dynamic_buf_push(&uart_rx_idle_data->dynbuf, uart_rx_idle_data->buf, Size);
	uart_rx_idle_data->dynbuf.busy = FALSE;
	uart_rx_idle_wait(uart_rx_idle_data);
}

void uart_rx_idle_read_dynbuf(uart_rx_idle_data_t * uart_rx_idle_data)
{
	size_t data_len;
	uint8_t buf[UART_RX_IDLE_BUF_SIZE];

	if (uart_rx_idle_data->dynbuf.busy) return;

	if (!dynamic_buf_pop(&uart_rx_idle_data->dynbuf, buf, sizeof(buf), &data_len)) return;

	if (data_len) uart_rx_idle_rcv_data(uart_rx_idle_data, buf, data_len);

}

void uart_rx_idle_task_data(uart_rx_idle_data_t * uart_rx_idle_data)
{

	if (dynamic_buf_is_empty(&uart_rx_idle_data->dynbuf)) return;

	if (uart_rx_idle_data->dynbuf.busy) return;

	uart_rx_idle_read_dynbuf(uart_rx_idle_data);
}


void uart_rx_idle_init(uart_rx_idle_data_t * uart_rx_idle_data, UART_HandleTypeDef *huart, size_t max_alloc_size)
{
	uart_rx_idle_data ->huart = huart;
	dynamic_buf_init(&uart_rx_idle_data->dynbuf, max_alloc_size);
	uart_rx_idle_wait(uart_rx_idle_data);
}

