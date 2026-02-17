/*
 * speex_util.h
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_SPEEX_UTIL_H_
#define UTIL_SPEEX_UTIL_H_

#include "def_util.h"
#include "speex_config_types.h"

void speex_init(void);
int speex_coder(spx_int16_t* in, char* bytes, int max_len);
int speex_decoder(const char* bytes, int len, spx_int16_t* out);
int speex_set_sampling_rate(int value);
bool spex20decodedac160(char* spex20, uint16_t* dac160);


#endif /* UTIL_SPEEX_UTIL_H_ */
