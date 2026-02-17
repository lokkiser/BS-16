/*
 * logfile_util.h
 *
 *  Created on: 15 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef LOGFILE_UTIL_H_
#define LOGFILE_UTIL_H_

#include "main.h"
#include "fatfs_util.h"

//добавить стоку в лог файл
FRESULT logfile_add(char * str);


#endif /* LOGFILE_UTIL_H_ */
