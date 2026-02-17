/*
 * upgrade_util.h
 *
 *  Created on: 18 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_UPGRADE_UTIL_H_
#define UTIL_UPGRADE_UTIL_H_

#include "main.h"

#define UPGRADE_FILE	"upgrade.bin"

void deinit(void);

bool check_loader(const char * loader_file_name, uint32_t loader_addr, uint8_t stm32flashtype);

void go_to_upgrade(const char * upgrade_file_name, const char * loader_file_name, uint32_t loader_addr, uint8_t stm32flashtype);

bool upgrade(const char * upgrade_file_name, uint8_t stm32flashtype, uint32_t addr);

//BOOL upgrade2(const char * upgrade_file_name, uint8_t stm32flashtype, uint32_t addr);

#endif /* UTIL_UPGRADE_UTIL_H_ */
