/*
 * dbg_util.h
 *
 *  Created on: 25 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_DBG_UTIL_H_
#define UTIL_DBG_UTIL_H_


#include <stdio.h>
#include "def_util.h"

void dbg_write(char * str); //должна быть определена!!!

typedef void dbg_get_time_t(char * buf, uint16_t buf_size);

void dbg_init(dbg_get_time_t * dbg_get_time_func);

void dbg_set_use_time(bool val);

void dbg_set_write_now(bool val);

void dbg(const char * fmt, ... );

void dbg_task(void);


#endif /* UTIL_DBG_UTIL_H_ */
