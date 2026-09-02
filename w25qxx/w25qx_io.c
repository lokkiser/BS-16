/*
 * w25qx_io.c
 *
 *  Created on: 24 апр. 2024 г.
 *      Author: Rafael Boltachev
 */


#include "flash_io.h"
#include "flash_io_conf.h"
#include "w25qx.h"

static W25QXX_HandleTypeDef w25qxx = {0};
static bool w25qxx_is_init = false;


bool flash_init(void)
{
	W25QXX_result_t W25QXX_result;
	uint8_t cnt;

	if (w25qxx_is_init) return true;

	cnt = 10;

	do 
	{
		W25QXX_result = w25qxx_init(&w25qxx, &FLASH_SPI_HANDLE, FLASH_CS_GPIO, FLASH_CS_PIN);
		cnt--;
		//TODO
		//HAL_IWDG_Refresh(&hiwdg);
	} while ((W25QXX_result != W25QXX_Ok) && cnt);

	//ulog_fmt("w25qxx_init()=%d"CRLF, W25QXX_result);

	if (W25QXX_result == W25QXX_Ok) w25qxx_is_init = true;

	return (W25QXX_result == W25QXX_Ok);
}



bool flash_read(uint8_t *buf, uint32_t sector, uint32_t count)
{

	W25QXX_result_t W25QXX_result;

	W25QXX_result = w25qxx_read(&w25qxx, sector * w25qxx.sector_size, buf , count * w25qxx.sector_size);

	return (W25QXX_result == W25QXX_Ok);
}


bool flash_write(uint8_t *buf,	uint32_t sector, uint32_t count)
{
	W25QXX_result_t W25QXX_result;

	W25QXX_result = w25qxx_erase(&w25qxx, sector * w25qxx.sector_size, count * w25qxx.sector_size);

	if (W25QXX_result != W25QXX_Ok) return false;

	W25QXX_result = w25qxx_write(&w25qxx, sector * w25qxx.sector_size, buf, count * w25qxx.sector_size);

	return (W25QXX_result == W25QXX_Ok);
}

uint32_t flash_sector_size(void)
{
	return w25qxx.sector_size;
}

uint32_t flash_sector_count(void)
{
	return w25qxx.sectors_count;
}

uint32_t flash_block_size(void)
{
	return w25qxx.block_size;
}


uint64_t flash_full_size(void)
{
	return w25qxx.sectors_count * w25qxx.sector_size;
}

uint32_t flash_block_count(void)
{
	return w25qxx.block_count;
}

uint16_t flash_device_id(void)
{
	return w25qxx.device_id;
}


