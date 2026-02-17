/*
 * oggopus_util.c
 *
 *  Created on: 7 мая 2025 г.
 *      Author: Rafael Boltachev
 */

#include "oggopus_util.h"
#include <string.h>



bool oggopus_open(ogg_decoder_t * ogg_decoder, uint32_t * sample_rate, uint8_t * channels_count)
{

	uint8_t segment[OPUS_MAX_ENCODE_SIZE];
	uint8_t segment_size;
	opus_head_t * opus_head;

	ogg_decoder->page_load = false;
	ogg_decoder->segment_index = 0;


	while (ogg_next_segment(ogg_decoder, segment, &segment_size) != OGG_RESULT_ERROR_READ_STREAM_EOFF) {


	     if ( (ogg_decoder->page.header_type == OGG_PAGE_HEADER_TYPE_BEGIN_OF_STREAM) &&
	          (ogg_decoder->page.number == 0) &&
	          (memcmp(segment, OGG_OPUS_HEAD_SIGNATURE, strlen(OGG_OPUS_HEAD_SIGNATURE)) == 0) ) {

	         opus_head = (opus_head_t *)segment;

	         *sample_rate = opus_head->sample_rate;
	         *channels_count = opus_head->channels_count;
	         //ulog_fmt("Opus head read, frequency:% lu, channels: %d"CRLF, opus_head->sample_rate, opus_head->channels_count);
	         return true;

	     }
	} //end while


	return false;
}

bool oggopus_open_file(FIL * file, const char * file_name, ogg_decoder_t * ogg_decoder,  uint32_t * sample_rate, uint8_t * channels_count)
{
	if (f_open(file, file_name, FA_READ) != FR_OK) {
	     //ulog_fmt("oggopus_open_file: error open file: %s"CRLF, file_name);
	     return false;
	}

	ogg_decoder->stream = file;
    ogg_decoder->stream_read = f_stream_read;
	return oggopus_open(ogg_decoder, sample_rate, channels_count);
}


uint16_t oggopus_read_segment(ogg_decoder_t * ogg_decoder, uint8_t * segment)
{
	  uint8_t segment_size;
	  uint16_t rc;


	  while (ogg_next_segment(ogg_decoder, segment, &segment_size) != OGG_RESULT_ERROR_READ_STREAM_EOFF) {


	    if (memcmp(segment, OGG_OPUS_TAGS_SIGNATURE, strlen(OGG_OPUS_TAGS_SIGNATURE)) == 0) {
	         continue;
	    }
	    else {

	    	if (!segment_size) continue;

    		rc = segment_size;
    		while (segment_size == 255) {
    			if (ogg_next_segment(ogg_decoder, segment + rc, &segment_size) == OGG_RESULT_ERROR_READ_STREAM_EOFF) break;
    			rc += segment_size;
    		}
    		return rc;
	    }

	  } //end while

	  return 0;
}

void oggopus_close_file(ogg_decoder_t * ogg_decoder)
{
	f_close((FIL * )ogg_decoder->stream);
}
