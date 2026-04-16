/*
 * akb.h
 *
 *  Created on: 26 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_AKB_H_
#define CORE_INC_AKB_H_

#include "main.h"

typedef enum
{
    NO_BATTERY = 0,
    BATTERY_LOOSE,
    BATTERY_CHARGED,
} battery_state_e;

void akb_task(void);
void akb_init(void);

bool akb_is_charged(void);
bool is_220_volts(void);
battery_state_e get_akb_state(void);

#endif /* CORE_INC_AKB_H_ */
