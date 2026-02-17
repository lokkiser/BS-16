/*
 * dac_util.h
 *
 *  Created on: 20 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_DAC_UTIL_H_
#define UTIL_DAC_UTIL_H_

#include <stdint.h>

//установить громкость данных DAC12
void dac12_set_volume(uint16_t  *dst_buf, uint16_t  *src_buf, uint16_t buf_len, uint16_t volume);
//вычислить мощность сигнала на DAC или ADC
uint16_t dac_calc_power(uint16_t *dac_buf, uint16_t dac_buf_len);

uint16_t dacdata_change_frequency(uint16_t *dst, uint32_t dst_frquency, uint16_t *src, uint16_t src_cnt, uint32_t src_frquency);
uint16_t dacdata_change_low_frequency_step(uint16_t *dst, uint16_t *src, uint16_t src_cnt, uint16_t step);


//установить значение DAC12
void dac12_set_value(uint16_t *dac_buf, uint16_t dac_buf_len, uint16_t value);

#endif /* UTIL_DAC_UTIL_H_ */
