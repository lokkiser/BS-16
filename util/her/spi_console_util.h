/*
 * spi_console_util.h
 *
 *  Created on: 12 мар. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef SPI_CONSOLE_UTIL_H_
#define SPI_CONSOLE_UTIL_H_

#include "main.h"

#define SPI_CONSOLE_BUF_SIZE	128

typedef void spi_console_rcv_func_t(uint8_t * data, uint16_t data_len);

typedef struct  _spi_console_data {
	SPI_HandleTypeDef *hspi;
	spi_console_rcv_func_t * rcv_func;
	uint8_t rcv_byte;
	uint16_t pos;
	uint32_t tick;
	uint8_t buf[SPI_CONSOLE_BUF_SIZE];
	uint8_t buf2[SPI_CONSOLE_BUF_SIZE];
} spi_console_data_t;





void spi_console_task_data(spi_console_data_t * spi_console_data);
void spi_console_callback_data(spi_console_data_t * spi_console_data);
void spi_console_wait(spi_console_data_t * spi_console_data);



#endif /* SPI_CONSOLE_UTIL_H_ */
