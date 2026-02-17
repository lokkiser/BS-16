/*
 * startafter_util.h
 *
 *  Created on: 12 сент. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_STARTAFTER_UTIL_H_
#define UTIL_STARTAFTER_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "def_util.h"


#define MAX_START_AFTER		10


typedef void on_start_after_function_t(void*);

typedef struct _start_after_data 
{
	on_start_after_function_t* on_start_after_function;
	void* param;
	uint32_t tick;
} start_after_data_t;


bool start_after(on_start_after_function_t* on_start_after_function, void* param, uint32_t interval);

uint32_t start_after_get_tick(void); //!neeed

void start_after_task(void);


#ifdef __cplusplus
}
#endif

#endif /* UTIL_STARTAFTER_UTIL_H_ */
