/*
 * exec_after.c
 *
 *  Created on: 15 мар. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "exec_after_util.h"

#include "vector_util.h"

static vector_t exec_after_vector;

typedef struct _exec_after_data {
        uint32_t id;
	exec_after_func_t *func;
	uint32_t timeout;
	uint32_t tick;
} exec_after_data_t;


void exec_after(exec_after_func_t func, uint32_t id, uint32_t timeout)
{
	exec_after_data_t exec_after_data;

	exec_after_data.func = func;
	exec_after_data.id = id;
	exec_after_data.timeout = timeout;
	exec_after_data.tick = exec_after_get_tick();

	vector_push(&exec_after_vector, &exec_after_data);
}

uint32_t exec_after_diff_tick(uint32_t old_tick)
{
   uint32_t new_tick = exec_after_get_tick();
   return new_tick < old_tick ? new_tick + (0xFFFFFFFF - old_tick) : new_tick - old_tick;
}

void exec_after_task(void)
{
	size_t i;
	exec_after_data_t * exec_after_data;

	if (!exec_after_vector.count) return;

	for (i = 0; i < vector_count(&exec_after_vector); ++i) {

		exec_after_data = (exec_after_data_t *)vector_get_at(&exec_after_vector, i);
		if (exec_after_diff_tick(exec_after_data->tick) > exec_after_data->timeout) {
			if (exec_after_data->func) exec_after_data->func(exec_after_data->id, FALSE);
			vector_delete_at(&exec_after_vector, i);
		}
	}

}

void exec_after_abort(uint32_t id)
{
	size_t i;
	exec_after_data_t * exec_after_data;

	for (i = 0; i < vector_count(&exec_after_vector); ++i) {

		exec_after_data = (exec_after_data_t *)vector_get_at(&exec_after_vector, i);
		if (exec_after_data->id == id) {
                        if (exec_after_data->func) exec_after_data->func(exec_after_data->id, TRUE);
			vector_delete_at(&exec_after_vector, i);
		}
	}
}


void exec_after_init(void)
{
	vector_init(&exec_after_vector, sizeof(exec_after_data_t));
}
