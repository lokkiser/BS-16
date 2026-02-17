/*
 * speex_util.c
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: Rafael Boltachev
 */


#include "speex_util.h"
#include "speex.h"
#include "stm_util.h"
#include "audio_util.h"
#include "stm_util.h"

#include "modes.h"

SpeexBits bits;/* Holds bits so they can be read and written by the Speex routines */
void *enc_state, *dec_state;/* Holds the states of the encoder & the decoder */
int quality = 4, complexity=1, vbr=0, enh=1;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */
int sampling_rate, enc_frame_size, dec_frame_size, skip_group_delay;

void speex_init(void)
{
  /* Speex encoding initializations */
  //ulog("1"CRLF);
  speex_bits_init(&bits);
  //ulog("2"CRLF);
  enc_state = speex_encoder_init(&speex_nb_mode);
  //ulog("3"CRLF);
  speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &vbr);
  //ulog("4"CRLF);
  speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY,&quality);
  //ulog("5"CRLF);
  speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &complexity);
  //ulog("6"CRLF);
  speex_encoder_ctl(enc_state, SPEEX_GET_SAMPLING_RATE, &sampling_rate);
  //ulog("7"CRLF);
  speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE, &enc_frame_size);
  //ulog("8"CRLF);
  speex_encoder_ctl(enc_state, SPEEX_GET_LOOKAHEAD, &skip_group_delay);
  //ulog("9"CRLF);

  /* speex decoding intilalization */
  dec_state = speex_decoder_init(&speex_nb_mode);
  //ulog("10"CRLF);
  speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);
  //ulog("11"CRLF);
  speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &dec_frame_size);
  //глюк декодера
  speex_decoder_ctl(dec_state, SPEEX_SET_INNOVATION_SAVE, NULL);

  //speex: init sr:8000 fs:160 sgd:40
  ulog_fmt("speex: init sr:%d efs:%d dfs:%d sgd:%d"CRLF, sampling_rate, enc_frame_size, dec_frame_size, skip_group_delay);


}


int speex_coder(spx_int16_t *in, char *bytes, int max_len)
{
  int rc;

  speex_bits_reset(&bits);
  // Encode the frame
  rc = speex_encode_int(enc_state, in, &bits);
  if (!rc) return 0;
  // Copy the bits to an array of char that can be decoded
  return speex_bits_write(&bits, bytes, max_len);
}


int speex_decoder(const char *bytes, int len, spx_int16_t * out)
{
  // Copy the encoded data into the bit-stream struct
 //ulog("1 ");
  speex_bits_read_from(&bits, bytes, len);
  // Decode the data
  //ulog("2");
  //int res = speex_decode_int(dec_state, &bits, out);
  //ulog("3");
  return speex_decode_int(dec_state, &bits, out);
}


int speex_set_sampling_rate(int value)
{
	return speex_encoder_ctl(enc_state, SPEEX_SET_SAMPLING_RATE, &value);
}


bool spex20decodepcm160(char * spex20, spx_int16_t * pcm160)
{
	if (speex_decoder(spex20, 20, pcm160) != 0) return FALSE;
	return TRUE;
}


bool spex20decodedac160(char * spex20, uint16_t * dac160)
{
	spx_int16_t pcm160[160];

	if (speex_decoder(spex20, 20, pcm160) != 0) return FALSE;

	pcmdata2dacdata(pcm160, dac160, 160, 100);

	return TRUE;
}
