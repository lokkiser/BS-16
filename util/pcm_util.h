/*
 * pcm_util.h
 *
 *  Created on: 16 февр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_PCM_UTIL_H_
#define UTIL_PCM_UTIL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//set pcm value
void pcm_set_value(int16_t *pcm_data, uint16_t pcm_data_len, int16_t value);
//set pcm volume
void pcm_set_volume(int16_t *dst_pcm_data, int16_t *src_pcm_data, uint16_t pcm_data_len, uint16_t volume);
//pcmь shift right (use on aLaw bits=4 , uLaw bits=3)
void pcm_set_shift_right(int16_t *dst_pcm_data, int16_t *src_pcm_data, uint16_t pcm_data_len, uint8_t bits);
//convert pcm16 to dac 12 bit data mono
void pcm_mono2dac12(int16_t * pcm_data, uint16_t * dac_data, uint16_t data_len);
//convert adc12 bit to pcm16 mono
void pcm_adc12mono2pcm(uint16_t * adc_data, int16_t * pcm_data, uint16_t data_len);
//sum 2 channels: left and right
uint16_t pcm_leftright2stereo(int16_t * pcm_left, uint16_t pcm_left_cnt, int16_t * pcm_right, uint16_t pcm_right_cnt, int16_t * pcm_stereo);
//stereo to left and right
uint16_t pcm_stereo2leftright(int16_t * pcm_stereo, uint16_t pcm_stereo_cnt, int16_t * pcm_left, int16_t * pcm_right);
//stereo to mono
uint16_t pcm_stereo2mono(int16_t * pcm_stereo, uint16_t pcm_stereo_cnt, int16_t * pcm_mono);
uint16_t pcm_mono2stereo(int16_t * pcm_mono, uint16_t pcm_mono_cnt, int16_t * pcm_stereo);
//change pcm frequency 8 nanosec
uint16_t pcm_change_frequency(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels);
//change pcm frequency 12 nanosec
uint16_t pcm_change_frequency2(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels);
//change pcm frequency 28 nanosec
uint16_t pcm_change_frequency_low_mono(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels);

uint16_t pcm_change_frequency_mono(int16_t *dst_mono, uint32_t dst_frquency, int16_t *src_mono, uint16_t src_cnt, uint32_t src_frquency);
//change pcm frequency 140 nanosec
uint16_t pcm_change_frequency_stereo(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency);


//change pcm frequency 77 nanosec
uint16_t pcm_change_frequency3(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels);

#ifdef __cplusplus
}
#endif


#endif /* UTIL_PCM_UTIL_H_ */
