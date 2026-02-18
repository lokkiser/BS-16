/*
 * mp3play.c
 *
 *  Created on: 6 мая 2024 г.
 *      Author: Rafael Boltachev
 */


#include "mp3read_util.h"
#include <string.h>
//#include <stdio.h>
//#include "pcm_util.h"
//#include "stm_util.h"
//#include "str_util.h"
#include "fatfs_util.h"






void mp3read_close(FIL * file, mad_data_t * amad_data)
{
	if (f_is_open(file)) {
		f_close(file);
		//ulog("mp3read_close"CRLF);
	}
	mad_data_clear(amad_data);
}


int mp3read_read_pcm16(FIL * file, mad_data_t *amad_data, int16_t * pcm16)
{
	int rc;

	if (!f_is_open(file)) return -1;


	rc = mad_read_next_frame(amad_data);
	if (rc == -1) {
		return -1;
	}
	if (rc != 1) return 0;
   	mad_synth_frame(&amad_data->synth, &amad_data->frame);
   	rc = mad_output_pcm16(&amad_data->synth.pcm, pcm16, MAX_MAD_PCM_SIZE);

   	return rc;
}

int mp3read_read_pcm16_mono(FIL * file, mad_data_t *amad_data, int16_t * pcm16)
{
	int rc;

	if (!f_is_open(file)) return -1;


	rc = mad_read_next_frame(amad_data);
	if (rc == -1) {
		return -1;
	}
	if (rc != 1) return 0;
   	mad_synth_frame(&amad_data->synth, &amad_data->frame);
   	rc = mad_output_pcm16_mono(&amad_data->synth.pcm, pcm16, MAX_MAD_PCM_SIZE);

   	return rc;
}


bool mp3read_open(FIL * file, mad_data_t *amad_data, const char * file_name)
{


	if (!strlen(file_name)) return false;

	mp3read_close(file, amad_data);

	mad_data_clear(amad_data);

	if (f_open(file, file_name, FA_READ) != FR_OK) {
	     //ulog_fmt("mp3read_open: error open file: %s"CRLF, file_name);
	     return false;
	}


	amad_data->file = file;
	amad_data->mad_input_read = f_stream_read;
	mad_stream_init(&amad_data->stream);
	mad_frame_init(&amad_data->frame);
	amad_data->frame.header.samplerate = 0;
	mad_synth_init(&amad_data->synth);
	amad_data->synth.pcm.length = 0;
	mad_stream_options(&amad_data->stream, 0); // TODO - add options support


    //ulog_fmt("mp3read_open: file:%s"CRLF, file_name);
	return true;
}


