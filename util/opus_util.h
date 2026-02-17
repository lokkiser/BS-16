/*
 * opus_util.h
 *
 *  Created on: 9 янв. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_OPUS_UTIL_H_
#define UTIL_OPUS_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

//max pcm frame size
#define OPUS_MAX_ENCODE_FRAME_SIZE			2880
//max pcm samples size (2880 * 2) , * 2 - for stereo
#define OPUS_MAX_ENCODE_PCM_SAMPLES_SIZE	(OPUS_MAX_ENCODE_FRAME_SIZE * 2)
//max encoded bytes
#define OPUS_MAX_ENCODE_SIZE 				1276

#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"


opus_int32 calc_opus_sampling_rate(int sampling_rate);


OpusDecoder * get_decoder_opus(void);

int decoder_opus_create(opus_int32 sampling_rate, int channels);
void decoder_opus_destroy(void);
int decode_opus(const unsigned char *data, opus_int32 len, opus_int16 *pcm, int frame_size);



OpusEncoder * get_encoder_opus(void);

int encoder_opus_create(opus_int32 sampling_rate, int channels);
void encoder_opus_destroy(void);

void encoder_opus_set_bitrate(int abitrate);
void encoder_opus_set_bandwidth(int abandwidth);

opus_int32 encode_opus(const opus_int16 *pcm, int frame_size, unsigned char *data, opus_int32 max_data_bytes);






#ifdef __cplusplus
}
#endif

#endif /* UTIL_OPUS_UTIL_H_ */
