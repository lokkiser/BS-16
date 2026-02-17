/*
 * timer_util.h
 *
 *  Created on: 27 июн. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_TIMER_UTIL_H_
#define UTIL_TIMER_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "def_util.h"



typedef void on_timer_function_t(void);

typedef struct _timer_data 
{
	on_timer_function_t * on_timer_function;
	uint32_t interval;
	uint32_t tick;
} timer_data_t;


bool timers_add(on_timer_function_t * on_timer_function, uint32_t interval);
bool timers_delete(on_timer_function_t on_timer_function);
void timers_clear(void);
void timers_task(void);

uint32_t timers_get_tick(void); //!neeed

#ifdef __cplusplus
}
#endif


#endif /* UTIL_TIMER_UTIL_H_ */
