/*
 * timer_util.c
 *
 *  Created on: 27 июн. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "timer_util.h"
#include "timer_conf.h"



#ifndef MAX_TIMERS
#define MAX_TIMERS		10
#endif

static timer_data_t timers_data[MAX_TIMERS] = {0};
static int timers_count = 0;

bool timers_delete(on_timer_function_t * on_timer_function)
{
	int i;
	bool res;

	i = 0;
	res = FALSE;
	while (i < timers_count) {
		if (timers_data[i].on_timer_function == on_timer_function) {
			timers_data[i] = timers_data[timers_count - 1];
			timers_count--;
			res = TRUE;
			continue;
		}
		i++;
	}
	return res;
}


#ifdef USE_FAST_TIMERS

bool timers_add(on_timer_function_t * on_timer_function, uint32_t interval)
{
    int i;

    if (!on_timer_function || !interval) return FALSE;

	if (timers_count >= MAX_TIMERS) return FALSE;

        for (i = 0; i < timers_count; ++i) {
          if (timers_data[i].on_timer_function == on_timer_function) {
             timers_data[i].interval = interval;
             timers_data[i].tick = timers_get_tick();
             timers_data[i].tick += interval;
             return TRUE;
          }
        }

	timers_data[timers_count].on_timer_function = on_timer_function;
	timers_data[timers_count].interval = interval;
	timers_data[timers_count].tick = timers_get_tick();
	timers_data[timers_count].tick += interval;
	timers_count++;
	return TRUE;
}




//timers_data[i].tick = будующие значения (1 ошибка при перескоке таймера в 4294967 сек или в 71582 мин или в 1193 час или в 49 суток)
void timers_task(void)
{
	int i;
	uint32_t now;

	i = 0;
	while (i < timers_count) {

        now = timers_get_tick();
		if (now > timers_data[i].tick) {
			timers_data[i].tick = now;
			timers_data[i].tick += timers_data[i].interval;
			timers_data[i].on_timer_function();
		}
		i++;

	}
}



#else

bool timers_add(on_timer_function_t * on_timer_function, uint32_t interval)
{
    int i;

    if (!on_timer_function || !interval) return FALSE;

	if (timers_count >= MAX_TIMERS) return FALSE;

        for (i = 0; i < timers_count; ++i) {
          if (timers_data[i].on_timer_function == on_timer_function) {
             timers_data[i].interval = interval;
             timers_data[i].tick = timers_get_tick();
             return TRUE;
          }
        }

	timers_data[timers_count].on_timer_function = on_timer_function;
	timers_data[timers_count].interval = interval;
	timers_data[timers_count].tick = timers_get_tick();
	timers_count++;
	return TRUE;
}



void timers_task(void)
{
	int i;
	uint32_t now;


	for (i = 0; i < timers_count; ++i) {

        now = timers_get_tick();
		if ((now < timers_data[i].tick ? (0xFFFFFFFF - timers_data[i].tick) + now : now - timers_data[i].tick)  > timers_data[i].interval) {
			timers_data[i].tick = now;
			timers_data[i].on_timer_function();
			//if (timers_data[i].on_timer_function) timers_data[i].on_timer_function();
		}

	}
}

#endif




void timers_clear(void)
{
  timers_count = 0;
}

