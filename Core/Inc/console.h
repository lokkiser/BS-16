/*
 * console.h
 *
 *  Created on: 17 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_CONSOLE_H_
#define CORE_INC_CONSOLE_H_

#include "main.h"

#define CONSOLE_UART	&huart3

void console_RxEventCallback(uint16_t Size);

void console_task(void);
void console_init(void);

#endif /* CORE_INC_CONSOLE_H_ */
