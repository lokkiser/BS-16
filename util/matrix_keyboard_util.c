/*
 * matrix_keyboard_util.c
 *
 *  Created on: 27 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "matrix_keyboard_util.h"
#include "matrix_keyboard_conf.h"


//.-------------------------------------------

//Matrix (bool) for keyboard old values
static uint8_t matrix_keyboard_values[MATRIX_KEYBOARD_OUT_CNT][MATRIX_KEYBOARD_IN_CNT] = {0};

//Index out
static uint8_t matrix_keyboard_out_index = 0xFF;



void matrix_keyboard_out_write(uint8_t index, uint8_t val)
{
	matrix_keyboard_write_out(index, val);
}

void matrix_keyboard_init(void)
{
	uint8_t o, i;

	for (o = 0; o < MATRIX_KEYBOARD_OUT_CNT; ++o) {
		matrix_keyboard_out_write(o, !MATRIX_KEYBOARD_OUT_ACTIVE);
	}

	for (o = 0; o < MATRIX_KEYBOARD_OUT_CNT; ++o) {
		for (i = 0; i < MATRIX_KEYBOARD_IN_CNT; ++i) {
			matrix_keyboard_values[o][i] = 0;
		}
	}


	matrix_keyboard_out_index = 0;
	matrix_keyboard_out_write(matrix_keyboard_out_index, MATRIX_KEYBOARD_OUT_ACTIVE);
}


void matrix_keyboard_task(void)
{
	uint8_t i, val;


	if (matrix_keyboard_out_index >= MATRIX_KEYBOARD_OUT_CNT) {
		matrix_keyboard_init();
		return;
	}

	for (i = 0; i < MATRIX_KEYBOARD_IN_CNT; ++i) {

		val = (matrix_keyboard_read_in(i) == MATRIX_KEYBOARD_IN_ACTIVE);

		if (!matrix_keyboard_values[matrix_keyboard_out_index][i] && val) matrix_keyboard_press(matrix_keyboard_out_index, i);
		else if (matrix_keyboard_values[matrix_keyboard_out_index][i] && !val) matrix_keyboard_unpress(matrix_keyboard_out_index, i);

		matrix_keyboard_values[matrix_keyboard_out_index][i] =	val;
	}

	matrix_keyboard_out_write(matrix_keyboard_out_index, !MATRIX_KEYBOARD_OUT_ACTIVE);


	matrix_keyboard_out_index++;
	if (matrix_keyboard_out_index >= MATRIX_KEYBOARD_OUT_CNT) { //finish all out

		matrix_keyboard_out_index = 0;
	}

	matrix_keyboard_out_write(matrix_keyboard_out_index, MATRIX_KEYBOARD_OUT_ACTIVE);
}


