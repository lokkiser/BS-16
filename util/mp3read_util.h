/*
 * mp3play.h
 *
 *  Created on: 6 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef MP3READ_UTIL_H_
#define MP3READ_UTIL_H_


//#include "main.h"
#include "ff.h"
#include "mad_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

//open mp3 file
bool mp3read_open(FIL * file, mad_data_t *amad_data, const char * file_name);
//pcm16 - with MAX_MAD_PCM_SIZE, return -1 if file eof
int mp3read_read_pcm16(FIL * file, mad_data_t *amad_data, int16_t * pcm16);
int mp3read_read_pcm16_mono(FIL * file, mad_data_t *amad_data, int16_t * pcm16);
//close mp3 file
void mp3read_close(FIL * file, mad_data_t *amad_data);


#ifdef __cplusplus
}
#endif



#endif /* MP3PLAY_UTIL_H_ */
