/*
 * rawopus.c
 *
 *  Created on: 14 февр. 2024 г.
 *      Author: Rafael Boltachev
 */
#include "rawopus_util.h"
#include <string.h>


uint8_t rawopus_open_file(const char * rawopus_file_name, FIL * fil, rawopus_hdr_t * rawopus_hdr)
{
	UINT br;

	if (!strlen(rawopus_file_name)) return RAWOPUS_OPEN_ERROR_PARAMS;

	if ( f_open(fil, rawopus_file_name, FA_READ) != FR_OK) return RAWOPUS_OPEN_ERROR_OPEN;

	if ( f_read(fil, rawopus_hdr, sizeof(rawopus_hdr_t),  &br) != FR_OK) {
		f_close(fil);
		return RAWOPUS_OPEN_ERROR_READ_HEADER;
	}

	if (br != sizeof(rawopus_hdr_t)) {
		f_close(fil);
		return RAWOPUS_OPEN_ERROR_HEADER_SIZE;
	}

	return RAWOPUS_OPEN_OK;
}

uint16_t rawopus_read_opus_data(FIL * fil, uint8_t * opus_data, uint16_t opus_data_size)
{

	UINT br;
	uint16_t data_len;


	if ( f_read(fil, &data_len, sizeof(data_len),  &br) != FR_OK) return 0;

	if (br != sizeof(data_len)) return 0;

	if (opus_data_size < data_len) 	return 0;

	if ( f_read(fil, opus_data, data_len,  &br) != FR_OK) return 0;

	if (br != data_len)	return 0;


	return data_len;
}

bool rawopus_write_opus_hdr(FIL * fil, rawopus_hdr_t * rawopus_hdr)
{
     UINT bw;

     if (f_write(fil, rawopus_hdr, sizeof(rawopus_hdr_t), &bw) != FR_OK) return false;
     if (bw != sizeof(rawopus_hdr_t)) return false;

     return true;
}

bool rawopus_write_opus_data(FIL * fil, uint8_t * opus_data, uint16_t opus_data_len)
{
     UINT bw;

     if (f_write(fil, &opus_data_len, sizeof(opus_data_len), &bw) != FR_OK) return false;
     if (bw != sizeof(opus_data_len)) return false;

     if (f_write(fil, opus_data, opus_data_len, &bw) != FR_OK) return 0;
     if (bw != opus_data_len) return false;

     return true;
}
