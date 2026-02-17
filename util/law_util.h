/*
 * alaw_util.h
 *
 *  Created on: 6 дек. 2023 г.
 *      Author: Rafael Boltachev
 *
 *      Библиотека для ALAW и ULAW
 */

#ifndef LAW_UTIL_H_
#define LAW_UTIL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void alaw2dac(int8_t * alaw_data, uint16_t * dac_data, uint16_t data_len, uint16_t volume);
void alaw2pcm(int8_t * alaw_data, int16_t * pcm_data, uint16_t data_len, uint16_t volume);
void adc2alaw(uint16_t * adc_data, int8_t * alaw_data, uint16_t data_len, uint16_t volume);
void pcm2alaw(int16_t * pcm_data, int8_t * alaw_data, uint16_t data_len, uint16_t volume);



void ulaw2dac(int8_t * ulaw_data, uint16_t * dac_data, uint16_t data_len, uint16_t volume);
void adc2ulaw(uint16_t * adc_data, int8_t * ulaw_data, uint16_t data_len, uint16_t volume);
void ulaw2pcm(int8_t * ulaw_data, int16_t * pcm_data, uint16_t data_len, uint16_t volume);
void pcm2ulaw(int16_t * pcm_data, int8_t * ulaw_data, uint16_t data_len, uint16_t volume);


#ifdef __cplusplus
}
#endif

#endif /* LAW_UTIL_H_ */
