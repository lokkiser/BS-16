/*
 * dac_util.c
 *
 *  Created on: 20 мая 2024 г.
 *      Author: Rafael Boltachev
 */
#include "dac_util.h"
#include "def_util.h"

void dac12_set_volume(uint16_t  *dst_buf, uint16_t  *src_buf, uint16_t buf_len, uint16_t volume)
{
	uint16_t i;
	int val;

	for (i = 0; i < buf_len; ++i) {
		val = 2047 + ((int)src_buf[i] - 2047) * volume / 100;
		dst_buf[i] = BOUNDSMALL(0, 4095, val);
	}
}


uint16_t dac_calc_power(uint16_t *dac_buf, uint16_t dac_buf_len)
{
	uint16_t i, ib1;
	uint32_t power_sum;

	power_sum = 0;

	for (i = 1; i < dac_buf_len; ++i) {
		ib1 = i - 1;
		if (dac_buf[i] > dac_buf[ib1]) power_sum += (dac_buf[i] - dac_buf[ib1]);
		else power_sum += (dac_buf[ib1] - dac_buf[i]);
	}
	return power_sum /  (dac_buf_len - 1);
}

uint16_t dacdata_change_frequency(uint16_t *dst, uint32_t dst_frquency, uint16_t *src, uint16_t src_cnt, uint32_t src_frquency)
{
  uint16_t dst_cnt, i, j;


  if (!src_cnt) return 0;

  dst_cnt = src_cnt * dst_frquency / src_frquency;


  for (i = 0; i < dst_cnt; i++) {
	  j = i * src_cnt / dst_cnt;
	  dst[i] = src[j];
  }

  return dst_cnt;
}


uint16_t dacdata_change_low_frequency_step(uint16_t *dst, uint16_t *src, uint16_t src_cnt, uint16_t step)
{
  uint16_t i, j;

  j = 0;
  for (i = 0; i < src_cnt; i += step) {
	  dst[j] = src[i];
	  j++;
  }

  return j;
}

void dac12_set_value(uint16_t *dac_buf, uint16_t dac_buf_len, uint16_t value)
{
	uint16_t i;

	for (i = 0; i < dac_buf_len; ++i) dac_buf[i] = value;
}
