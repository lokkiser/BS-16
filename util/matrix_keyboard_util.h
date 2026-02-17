/*
 * matrix_keyboard_util.h
 *
 *  Created on: 27 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_MATRIX_KEYBOARD_UTIL_H_
#define UTIL_MATRIX_KEYBOARD_UTIL_H_

#include <stdint.h>


//init
void matrix_keyboard_init(void);
//need to be called periodically
void matrix_keyboard_task(void);

//must be defined in your program
void matrix_keyboard_press(uint8_t out, uint8_t in);
//must be defined in your program
void matrix_keyboard_unpress(uint8_t out, uint8_t in);

#endif /* UTIL_MATRIX_KEYBOARD_UTIL_H_ */
