/*
 * w25qx_io.h
 *
 *  Created on: 24 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef FLASH_IO_H_
#define FLASH_IO_H_

#include "main.h"


bool flash_init(void);

bool flash_write(uint8_t* buf,	uint32_t sector, uint32_t count);

bool flash_read(uint8_t* buf, uint32_t sector, uint32_t count);

uint32_t flash_sector_size(void);

uint32_t flash_sector_count(void);

uint32_t flash_block_size(void);

uint64_t flash_full_size(void);

uint32_t flash_block_count(void);

uint16_t flash_device_id(void);

#endif /* FLASH_IO_H */
