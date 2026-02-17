/*
 * spi_console_util.c
 *
 *  Created on: 12 мар. 2024 г.
 *      Author: Rafael Boltachev
 */


#include "spi_console_util.h"
#include "stm_util.h"
#include <string.h>
//#include "fulog.h"



void spi_console_wait(spi_console_data_t * spi_console_data)
{
	HAL_SPI_Receive_IT(spi_console_data->hspi, &spi_console_data->rcv_byte, 1);
}

void spi_console_callback_data(spi_console_data_t * spi_console_data)
{
	if (spi_console_data->pos >= sizeof(spi_console_data->buf)) spi_console_data->pos = 0;
	spi_console_data->buf[spi_console_data->pos] = spi_console_data->rcv_byte;
	spi_console_data->tick = HAL_GetTickNoZero(); //таймер на timeout
	spi_console_data->pos++; //добовляем позицию
	spi_console_wait(spi_console_data);
}



void spi_console_rcv_data(spi_console_data_t * spi_console_data, uint16_t pos)
{

	if (pos) {
		memcpy(spi_console_data->buf2, spi_console_data->buf, pos);
	}

	spi_console_data->pos = 0;
	spi_console_data->tick = 0;

	if (pos) {
		 if (spi_console_data->rcv_func) spi_console_data->rcv_func(spi_console_data->buf2, pos);
	}
}


void spi_console_task_data(spi_console_data_t * spi_console_data)
{
	if (spi_console_data->tick) {
		if ( HAL_DiffTick(spi_console_data->tick) > 10) {
			 spi_console_rcv_data(spi_console_data, spi_console_data->pos);
		}
	}
}

