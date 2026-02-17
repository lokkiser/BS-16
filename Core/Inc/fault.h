/*
 * fault.h
 *
 *  Created on: 29 дек. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_FAULT_H_
#define CORE_INC_FAULT_H_

#include "main.h"


#define FAULT_OUT_ADC1_KZ			0
#define FAULT_OUT_ADC2_KZ			1
#define FAULT_OUT_ADC3_KZ			2
#define FAULT_OUT_ADC4_KZ			3
#define FAULT_OUT_ADC5_KZ			4
#define FAULT_OUT_ADC6_KZ			5
#define FAULT_OUT_ADC7_KZ			6
#define FAULT_OUT_ADC8_KZ			7
#define FAULT_OUT_ADC9_KZ			8
#define FAULT_OUT_ADC10_KZ			9
#define FAULT_OUT_ADC11_KZ			10
#define FAULT_OUT_ADC12_KZ			11
#define FAULT_OUT_ADC13_KZ			12
#define FAULT_OUT_ADC14_KZ			13
#define FAULT_OUT_ADC15_KZ			14
#define FAULT_OUT_ADC16_KZ			15

#define FAULT_OUT_ADC1_OBRIV		16
#define FAULT_OUT_ADC2_OBRIV		17
#define FAULT_OUT_ADC3_OBRIV		18
#define FAULT_OUT_ADC4_OBRIV		19
#define FAULT_OUT_ADC5_OBRIV		20
#define FAULT_OUT_ADC6_OBRIV		21
#define FAULT_OUT_ADC7_OBRIV		22
#define FAULT_OUT_ADC8_OBRIV		23
#define FAULT_OUT_ADC9_OBRIV		24
#define FAULT_OUT_ADC10_OBRIV		25
#define FAULT_OUT_ADC11_OBRIV		26
#define FAULT_OUT_ADC12_OBRIV		27
#define FAULT_OUT_ADC13_OBRIV		28
#define FAULT_OUT_ADC14_OBRIV		29
#define FAULT_OUT_ADC15_OBRIV		30
#define FAULT_OUT_ADC16_OBRIV		31



#define FAULT_P_OUT_1_KZ			32
#define FAULT_P_OUT_2_KZ			33
#define FAULT_P_OUT_3_KZ			34
#define FAULT_P_OUT_4_KZ			35
#define FAULT_P_OUT_5_KZ			36
#define FAULT_P_OUT_6_KZ			37
#define FAULT_P_OUT_7_KZ			38
#define FAULT_P_OUT_8_KZ			39
#define FAULT_P_OUT_9_KZ			40
#define FAULT_P_OUT_10_KZ			41
#define FAULT_P_OUT_11_KZ			42
#define FAULT_P_OUT_12_KZ			43
#define FAULT_P_OUT_13_KZ			44
#define FAULT_P_OUT_14_KZ			45
#define FAULT_P_OUT_15_KZ			46
#define FAULT_P_OUT_16_KZ			47

#define FAULT_P_OUT_1_OBRIV			48
#define FAULT_P_OUT_2_OBRIV			49
#define FAULT_P_OUT_3_OBRIV			50
#define FAULT_P_OUT_4_OBRIV			51
#define FAULT_P_OUT_5_OBRIV			52
#define FAULT_P_OUT_6_OBRIV			53
#define FAULT_P_OUT_7_OBRIV			54
#define FAULT_P_OUT_8_OBRIV			55
#define FAULT_P_OUT_9_OBRIV			56
#define FAULT_P_OUT_10_OBRIV		57
#define FAULT_P_OUT_11_OBRIV		58
#define FAULT_P_OUT_12_OBRIV		59
#define FAULT_P_OUT_13_OBRIV		60
#define FAULT_P_OUT_14_OBRIV		61
#define FAULT_P_OUT_15_OBRIV		62
#define FAULT_P_OUT_16_OBRIV		63

#define FAULT_HARDWARE_ERROR		64

bool set_fault(uint8_t number);
bool unset_fault(uint8_t number);
bool is_set_fault(uint8_t number);


#endif /* CORE_INC_FAULT_H_ */
