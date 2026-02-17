/*
 * mad_util.h
 *
 *  Created on: 27 окт. 2023 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_MAD_UTIL_H_
#define UTIL_MAD_UTIL_H_

#include <stdint.h>
#include <stdio.h>
#include "mad.h"
#include "def_util.h"



#ifdef __cplusplus
extern "C" {
#endif


#ifndef MIN
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif

//max PCM size in frame 2304
#ifndef MAX_MAD_PCM_SIZE
#define MAX_MAD_PCM_SIZE  2500
#endif

//1024 small, 2048 norm
#ifndef MAD_INPUT_BUFFER_SIZE
#define MAD_INPUT_BUFFER_SIZE   2048
#endif


//typedef size_t mad_input_read_t(void * file, unsigned char * read_start, size_t read_size);
typedef f_stream_read_t mad_input_read_t;


typedef struct _mad_data {
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
    unsigned char input_buffer[MAD_INPUT_BUFFER_SIZE];
    void * file;
    mad_input_read_t * mad_input_read;
} mad_data_t;

// Reads the next frame from the file.  Returns 1 on success or 0 on failure or -1 on input_read error.
int mad_read_next_frame(mad_data_t * pmad_data);

//scale to PCM32
int mad_scale_int32(mad_fixed_t sample);

//scale to PCM16
int16_t mad_scale_int16(mad_fixed_t sample);
//mad_pcm to PCM16
int mad_output_pcm16(struct mad_pcm *pcm, int16_t * pcm16, int pcm16_size);
//mad_pcm to PCM16 mono
int mad_output_pcm16_mono(struct mad_pcm *pcm, int16_t * pcm16, int pcm16_size);

//scale to PCM16(for LAW)
int16_t mad_scale_int16_law(mad_fixed_t sample);
//mad_pcm to PCM16(for LAW)
int mad_output_pcm16_mono_law(struct mad_pcm *pcm, int16_t * pcm16, int pcm16_size);

//scale to DAC12
uint16_t mad_scale_dac12(mad_fixed_t sample);
//mad_pcm to DAC12
int mad_output_dac12_mono(struct mad_pcm *pcm, uint16_t * dac12, int dac12_size);

//clear mad_data
void mad_data_clear(mad_data_t * pmad_data);

extern mad_data_t global_mad_data;

#ifdef __cplusplus
}
#endif

#endif /* UTIL_MAD_UTIL_H_ */
