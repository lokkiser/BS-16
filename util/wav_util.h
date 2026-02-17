/*
 * wav_util.h
 *
 *  Created on: 6 дек. 2023 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_WAV_UTIL_H_
#define UTIL_WAV_UTIL_H_

#include <stdint.h>
#include "def_util.h"

#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push, 1)

typedef struct _wav_header
{
	char riff[4];  //строка RIFF
	long size;     //размер данных от wave до конца файла (или длина файла - 8)
	char wave[4];  //строка WAVE

	char fmt[4];   //строка fmt
	long fmt_size;  //размер чанка  16
	short int audio_fmt; //формат данных
	short int channel; //количество каналов
	long int sampling_f; //Частота дискретизации(количество отсчетов одного канала в секунду).
	long int byte_rate; //байт/сек примерное количество байтов в секунду (скорость звукового потока)
	short int block_align; //выравнивание данных 1 (количество байтов, занимаемое одним блоком звукового потока)
	short int bits_per_sample; //разрядность выборки 8
} wav_header_t;


typedef struct _wav_chunk
{
    char id[4]; //метка чанка
    long id_size; //размер чанка
} wav_chunk_t;


#pragma pack(pop)


#define WAV_OK						0
#define WAV_ERROR_OPEN_PARAMS		1
#define WAV_ERROR_OPEN_FILE			2
#define WAV_ERROR_READ_HEADER		3
#define WAV_ERROR_HEADER_FORMAT		4
#define WAV_ERROR_HEADER_PARAMS		5
#define WAV_ERROR_READ_CHUNK		6

const char * waverr2str(uint8_t err);

//открыть WAV файл
uint8_t wavopenfile(const char * wav_file_name, FIL * a_file, wav_header_t * a_file_hdr);

//открыть WAV файл 2
bool wav_open_file(const char * wav_file_name, FIL * a_file, wav_header_t * a_file_hdr);

//из данных WAV получить PCM 16 бит значения
uint16_t wav2pcm16(uint8_t * wav_data, uint16_t wav_data_cnt, int16_t * pcm_data, uint16_t pcm_data_len, uint8_t bits_per_sample);

//из WAV файла получить PCM 16 бит значения
uint16_t wavfile2pcm16(FIL * file, int16_t * pcm_data, uint16_t pcm_data_len, uint8_t bits);


//голичество сэмплов (точек осцилограммы)
int wav_get_num_samples(wav_header_t * hdr, long id_size);

//длительность в милисекундах
int wav_get_duration_ms(wav_header_t * hdr, long id_size);


uint16_t wavdata2dacdata(uint8_t * wav_data, uint16_t wav_data_cnt, uint16_t * dac_data,  uint8_t bits_per_sample, uint8_t channel_cnt, uint16_t volume);

uint16_t wavfile2dacdata(FIL * file, uint16_t * dacdata, uint16_t dacdata_len, uint8_t bits, uint8_t chan, uint16_t volume);

//uint16_t wavfile2pcmdata(FIL * file, int16_t * pcmdata, uint16_t pcmdata_len, uint8_t bits, uint8_t chan, uint16_t volume);

//uint16_t wavfile2alawdata(FIL * file, int8_t * alawdata, uint16_t alawdata_len, uint8_t bits, uint8_t chan, uint16_t volume);

uint16_t wavfile2pcmdata2(FIL * file, int16_t * pcmdata, uint16_t pcmdata_len, uint8_t bits, uint8_t chan, uint16_t volume);






#ifdef __cplusplus
}
#endif


#endif /* UTIL_WAV_UTIL_H_ */
