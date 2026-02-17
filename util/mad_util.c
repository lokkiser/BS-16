/*
 * mad_util.c
 *
 *  Created on: 27 окт. 2023 г.
 *      Author: Rafael Boltachev
 */

#include "mad_util.h"
#include <string.h>

//#include "stm_util.h"


mad_data_t global_mad_data = {0};

// Some helper functions, to be cleaned up in the future
int mad_scale_int32(mad_fixed_t sample)
{
     // round
     sample += (1L << (MAD_F_FRACBITS - 16));
     // clip
     if (sample >= MAD_F_ONE)
         sample = MAD_F_ONE - 1;
     else if (sample < -MAD_F_ONE)
         sample = -MAD_F_ONE;
     // quantize
     return sample >> (MAD_F_FRACBITS + 1 - 16);
}

//scale DAC12
uint16_t mad_scale_dac12(mad_fixed_t sample) {
  int val;

  val = 2047 + (sample >> 17);
  return val < 0 ? 0 : val > 4095 ? 4095 : val;
}

//scale PCM16
int16_t mad_scale_int16(mad_fixed_t sample)
{
  return sample >> 13;
}

int mad_output_pcm16(struct mad_pcm *pcm, int16_t * pcm16, int pcm16_size)
{
   unsigned int nsamples;
   mad_fixed_t const *left_ch;
   mad_fixed_t const *right_ch;
   int i;


   left_ch   = pcm->samples[0];
   right_ch  = pcm->samples[1];

   i = 0;

   if (pcm->channels == 2) {

     nsamples  = MIN(pcm->length,  pcm16_size / 2);
     while (nsamples--) {
       pcm16[i] = mad_scale_int16(*left_ch++);
       i++;
       pcm16[i] = mad_scale_int16(*right_ch++);
       i++;
     }

   } else {

     nsamples  = MIN(pcm->length,  pcm16_size);
     while (nsamples--) {
       pcm16[i] = mad_scale_int16(*left_ch++);
       i++;
     }

   }
   return i;
}


int mad_output_pcm16_mono(struct mad_pcm *pcm, int16_t * pcm16, int pcm16_size)
{
   unsigned int nsamples;
   mad_fixed_t const *left_ch;
   mad_fixed_t const *right_ch;
   int i;


   left_ch   = pcm->samples[0];
   right_ch  = pcm->samples[1];

   i = 0;

   nsamples  = MIN(pcm->length,  pcm16_size);

   if (pcm->channels == 2) {

     while (nsamples--) {
       pcm16[i] = ((int32_t)mad_scale_int16(*left_ch++) + (int32_t)mad_scale_int16(*right_ch++)) / 2;
       i++;
     }

   } else {

     while (nsamples--) {
       pcm16[i] = mad_scale_int16(*left_ch++);
       i++;
     }

   }
   return i;
}


int mad_output_dac12_mono(struct mad_pcm *pcm, uint16_t * dac12, int dac12_size)
{
   unsigned int nsamples;
   mad_fixed_t const *left_ch;
   mad_fixed_t const *right_ch;
   int i;


   left_ch   = pcm->samples[0];
   right_ch  = pcm->samples[1];

   i = 0;

   nsamples  = MIN(pcm->length,  dac12_size);

   if (pcm->channels == 2) {

     while (nsamples--) {
       dac12[i] = (mad_scale_dac12(*left_ch++) + mad_scale_dac12(*right_ch++)) / 2;
       i++;
     }

   } else {

     while (nsamples--) {
       dac12[i] = mad_scale_dac12(*left_ch++);
       i++;
     }

   }
   return i;
}

//scale to LAW
int16_t mad_scale_int16_law(mad_fixed_t sample)
{
  return sample >> 17;
}

int mad_output_pcm16_mono_law(struct mad_pcm *pcm, int16_t * pcm16, int pcm16_size)
{
   unsigned int nsamples;
   mad_fixed_t const *left_ch;
   mad_fixed_t const *right_ch;
   int i;


   left_ch   = pcm->samples[0];
   right_ch  = pcm->samples[1];

   i = 0;

   nsamples  = MIN(pcm->length,  pcm16_size);

   if (pcm->channels == 2) {

     while (nsamples--) {
       pcm16[i] = (mad_scale_int16_law(*left_ch++) + mad_scale_int16_law(*right_ch++)) / 2;
       i++;
     }

   } else {

     while (nsamples--) {
       pcm16[i] = mad_scale_int16_law(*left_ch++);
       i++;
     }

   }
   return i;
}

void mad_data_clear(mad_data_t * pmad_data)
{
	mad_stream_finish(&pmad_data->stream);
	mad_frame_finish(&pmad_data->frame);
	mad_synth_finish(&pmad_data->synth);

//	memset(mad_data, 0, sizeof(mad_data_t));
}


// Reads the next frame from the file.  Returns 1 on success or 0 on failure or -1 on input_read error or frame error.
int mad_read_next_frame(mad_data_t *pmad_data) {

  size_t read_size;
  size_t remaining;
  unsigned char *read_start;

  //ulog("100"CRLF);
  if (pmad_data->stream.buffer == NULL || pmad_data->stream.error == MAD_ERROR_BUFLEN) {
	  //ulog("200"CRLF);
	  // There might be some bytes in the buffer left over from last
	  // time.  If so, move them down and read more bytes following
	  // them.
	  if (pmad_data->stream.next_frame != NULL) {
		  //ulog("300"CRLF);
		  remaining = pmad_data->stream.bufend - pmad_data->stream.next_frame;
		  memmove(pmad_data->input_buffer, pmad_data->stream.next_frame, remaining);
		  read_start = pmad_data->input_buffer + remaining;
		  read_size = MAD_INPUT_BUFFER_SIZE - remaining;
	  } else {
		  //ulog("400"CRLF);
		  read_size = MAD_INPUT_BUFFER_SIZE;
		  read_start = pmad_data->input_buffer;
		  remaining = 0;
	  }
	  // Now read additional bytes from the input file.
	  //ulog("500"CRLF);
	  read_size = pmad_data->mad_input_read(pmad_data->file, read_start, read_size);
	  if (read_size == 0) {
		  //ulog("600"CRLF);
		  return -1;
	  }
	  // Now feed those bytes into the libmad stream.
	  mad_stream_buffer(&pmad_data->stream, pmad_data->input_buffer, read_size + remaining);
	  pmad_data->stream.error = MAD_ERROR_NONE;
  }
  // Now ask libmad to extract a frame from the data we just put in
  // its buffer.
  if (mad_frame_decode(&pmad_data->frame, &pmad_data->stream) == -1) {
	  if (MAD_RECOVERABLE(pmad_data->stream.error)) {
		  //ulog("700"CRLF);
		  return 0;
	  } else if (pmad_data->stream.error == MAD_ERROR_BUFLEN) {
		  //ulog("800"CRLF);
		  return 0;
	  } else {
		  //ulog("900"CRLF);
		  return -1;
	  }
  }

//  ulog("1000"CRLF);
  return 1;
}
