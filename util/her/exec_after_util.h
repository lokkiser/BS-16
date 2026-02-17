/*
 * exec_after.h
 *
 *  Created on: 15 мар. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_EXEC_AFTER_H_
#define CORE_INC_EXEC_AFTER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
#ifndef BOOL
#define BOOL	unsigned char
#endif
*/
typedef void exec_after_func_t(uint32_t id, bool abort);
void exec_after(exec_after_func_t func, uint32_t id, uint32_t timeout);
void exec_after_task(void);
void exec_after_abort(uint32_t id);
void exec_after_init(void);

uint32_t exec_after_get_tick(void);


#ifdef __cplusplus
}
#endif


#endif /* CORE_INC_EXEC_AFTER_H_ */
