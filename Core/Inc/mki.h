/*
 * mki.h
 *
 *  Created on: 15 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_MKI_H_
#define CORE_INC_MKI_H_

#include "main.h"

#define MKI_I2C_HANDLE		&hi2c4
#define MKI_DD1_ADDERESS	(0x00)
#define MKI_DD2_ADDERESS	(0x01)
#define MKI_DD3_ADDERESS	(0x02)
#define MKI_DD4_ADDERESS	(0x03)
#define MKI_DD5_ADDERESS	(0x20)
#define MKI_DD6_ADDERESS	(0x21)


#define MKI_LED_COLOR_BLACK			0x00
#define MKI_LED_COLOR_GREEN			0x01
#define MKI_LED_COLOR_BLUE			0x02
#define MKI_LED_COLOR_LIGHTBLUE		0x03
#define MKI_LED_COLOR_RED			0x04
#define MKI_LED_COLOR_YELLOW		0x05
#define MKI_LED_COLOR_VIOLET		0x06
#define MKI_LED_COLOR_WHITE			0x07


#define MKI_LED_1					1
#define MKI_LED_2					2
#define MKI_LED_3					3
#define MKI_LED_4					4
#define MKI_LED_5					5
#define MKI_LED_6					6
#define MKI_LED_7					7
#define MKI_LED_8					8
#define MKI_LED_9					9
#define MKI_LED_10					10
#define MKI_LED_11					11
#define MKI_LED_12					12
#define MKI_LED_13					13
#define MKI_LED_14					14
#define MKI_LED_15					15
#define MKI_LED_16					16
#define MKI_LED_220					17
#define MKI_LED_BAT					18
#define MKI_LED_AVAR				19
#define MKI_LED_START				20

uint8_t mki_get_led_color(uint8_t number);
void mki_set_led_color(uint8_t number, uint8_t color);
void mki_init(void);
void mki_unpress(uint8_t key);
void mki_press(uint8_t key);
void mki_task(void);


#define MKI_KEY_1					1
#define MKI_KEY_2					2
#define MKI_KEY_3					3
#define MKI_KEY_4					4
#define MKI_KEY_5					5
#define MKI_KEY_6					6
#define MKI_KEY_7					7
#define MKI_KEY_8					8
#define MKI_KEY_9					9
#define MKI_KEY_10					10
#define MKI_KEY_11					11
#define MKI_KEY_12					12
#define MKI_KEY_13					13
#define MKI_KEY_14					14
#define MKI_KEY_15					15
#define MKI_KEY_16					16
#define MKI_KEY_ZVUKOVOI_VIZOV		17
#define MKI_KEY_KONEC_SVAZI			18

extern uint32_t dd5_change_tick, dd6_change_tick;

void mki_test(void);

#endif /* CORE_INC_MKI_H_ */
