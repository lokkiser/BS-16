/*
 * fault.c
 *
 *  Created on: 29 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#include "fault.h"
#include "math_util.h"

uint8_t faults[128 / 8] = {0};


bool is_fault(void)
{
    for (uint16_t i = 0; i < (8*sizeof(faults)); ++i)
    {
        if(bitget(faults, i)) return true;
    }

    return false;
}

bool is_set_fault(uint8_t number)
{
	return bitget(faults, number);
}

bool set_fault(uint8_t number)
{
	if (bitget(faults, number)) return false;

	bitset(faults, number, 1);
	return true;
}

bool unset_fault(uint8_t number)
{
	if (!bitget(faults, number)) return false;

	bitset(faults, number, 0);
	return true;
}
