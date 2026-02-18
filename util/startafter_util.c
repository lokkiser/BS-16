/*
 * startafter_util.c
 *
 *  Created on: 12 сент. 2025 г.
 *      Author: Rafael Boltachev
 */

#include "startafter_util.h"

static start_after_data_t start_after_data[MAX_START_AFTER] = {0};
static int start_after_count = 0;

bool start_after(on_start_after_function_t * on_start_after_function, void * param, uint32_t interval)
{
    int i;

    if (!on_start_after_function || !interval) return false;

	if (start_after_count >= MAX_START_AFTER) return false;

        for (i = 0; i < start_after_count; ++i) {
          if (start_after_data[i].on_start_after_function == on_start_after_function) {
        	  start_after_data[i].param = param;
        	  start_after_data[i].tick = start_after_get_tick();
        	  start_after_data[i].tick += interval;
        	  return true;
          }
        }

    start_after_data[start_after_count].on_start_after_function = on_start_after_function;
	start_after_data[start_after_count].param = param;
    start_after_data[start_after_count].tick = start_after_get_tick();
    start_after_data[start_after_count].tick += interval;
    start_after_count++;
	return true;
}


void start_after_task(void)
{
	int i;

	i = 0;
	while (i < start_after_count) {

		if (start_after_get_tick() > start_after_data[i].tick) {
			start_after_data[i].on_start_after_function(start_after_data[i].param);
			start_after_data[i] = start_after_data[start_after_count - 1];
			start_after_count--;
		}
		i++;
	}
}

