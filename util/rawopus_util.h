/*
 * rawopus.h
 *
 *  Created on: 14 февр. 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_RAWOPUS_UTIL_H_
#define UTIL_RAWOPUS_UTIL_H_

#include <stdint.h>
#include "ff.h"
#include "def_util.h"

#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push, 1)

typedef struct _rawopus_hdr {
	unsigned int frequency;
	unsigned char channels;
} rawopus_hdr_t;

#pragma pack(pop)

#define RAWOPUS_OPEN_OK					0
#define RAWOPUS_OPEN_ERROR_PARAMS		1
#define RAWOPUS_OPEN_ERROR_OPEN			2
#define RAWOPUS_OPEN_ERROR_READ_HEADER	3
#define RAWOPUS_OPEN_ERROR_HEADER_SIZE	4

uint8_t rawopus_open_file(const char * rawopus_file_name, FIL * fil, rawopus_hdr_t * rawopus_hdr);
uint16_t rawopus_read_opus_data(FIL * fil, uint8_t * opus_data, uint16_t opus_data_size);

bool rawopus_write_opus_hdr(FIL * fil, rawopus_hdr_t * rawopus_hdr);
bool rawopus_write_opus_data(FIL * fil, uint8_t * opus_data, uint16_t opus_data_len);

#ifdef __cplusplus
}
#endif


#endif /* UTIL_RAWOPUS_UTIL_H_ */
