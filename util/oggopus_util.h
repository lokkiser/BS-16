/*
 * oggopus_util.h
 *
 *  Created on: 7 мая 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_OGGOPUS_UTIL_H_
#define UTIL_OGGOPUS_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fatfs_util.h"
#include "ogg_util.h"

#ifndef OPUS_MAX_ENCODE_SIZE
#define OPUS_MAX_ENCODE_SIZE 				1276
#endif

//open opus file
bool oggopus_open(ogg_decoder_t * ogg_decoder, uint32_t * sample_rate, uint8_t * channels_count);
uint16_t oggopus_read_segment(ogg_decoder_t * ogg_decoder, uint8_t * segment);

bool oggopus_open_file(FIL * file, const char * file_name, ogg_decoder_t * ogg_decoder,  uint32_t * sample_rate, uint8_t * channels_count);
void oggopus_close_file(ogg_decoder_t * ogg_decoder);


#ifdef __cplusplus
}
#endif


#endif /* UTIL_OGGOPUS_UTIL_H_ */
