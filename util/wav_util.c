/*
 * wav_util.c
 *
 *  Created on: 6 дек. 2023 г.
 *      Author: Rafael Boltachev
 */

#include "wav_util.h"
#include <string.h>

//#ifndef USE_FATFS_IMITATION
//#include "stm_util.h"
//#include "audio_util.h"
//#endif


//голичество сэмплов (точек осцилограммы)
int wav_get_num_samples(wav_header_t * hdr, long id_size)
{
  return (id_size * 8)/(hdr->channel * hdr->bits_per_sample);
}

//длительность в милисекундах
int wav_get_duration_ms(wav_header_t * hdr, long id_size)
{
	return wav_get_num_samples(hdr, id_size) * 1000 / hdr->sampling_f;
}


//Из WAV данных в DAC данные, возращает dac_data_cnt
uint16_t wavdata2dacdata(uint8_t * wav_data, uint16_t wav_data_cnt, uint16_t * dac_data,  uint8_t bits_per_sample, uint8_t channel_cnt, uint16_t volume)
{
	uint16_t i, dac_data_cnt;
	int32_t data;
	int16_t * wav_data16;
    uint8_t * wav_data8;
	int32_t * wav_data32;

	if ( (bits_per_sample == 8) && (channel_cnt == 1) ) { //8 бит моно  OK
		dac_data_cnt = wav_data_cnt;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = wav_data[i] << 4; //надо на 4
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}
		} else {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = 2047 + ((int16_t)(wav_data[i] << 4) - 2047) * volume / 100;
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}
		}
		return dac_data_cnt;
	}

	if ( (bits_per_sample == 8) && (channel_cnt == 2) ) { //8 бит стерео OK
		dac_data_cnt = wav_data_cnt / 2;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = wav_data[i * 2] << 4; //надо на 4
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}
		} else {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = 2047 + ((int16_t)(wav_data[i * 2] << 4) - 2047) * volume / 100;
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}

		}
		return dac_data_cnt;
	}


	if ( (bits_per_sample == 16) && (channel_cnt == 1) ) { //16 бит моно OK
		dac_data_cnt = wav_data_cnt / 2;
		wav_data16 = (int16_t *)wav_data;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = (wav_data16[i] + 32768) >> 4;
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}
		} else {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = 2047 + (((wav_data16[i] + 32768) >> 4) - 2047) * volume / 100;
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}

		}
		return dac_data_cnt;
	}

	if ( (bits_per_sample == 16) && (channel_cnt == 2) ) { //16 бит стерео
		wav_data16 = (int16_t *)wav_data;
		dac_data_cnt = wav_data_cnt / 4;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; i++) {
				data = (wav_data16[i * 2] + 32768) >> 4;
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}
		} else {
			for (i = 0; i < dac_data_cnt; i++) {
				data = 2047 + (((wav_data16[i * 2] + 32768) >> 4) - 2047) * volume / 100;
				dac_data[i] = BOUNDSMALL(0, 4095, data);
			}

		}
		return dac_data_cnt;
	}


	if ((bits_per_sample == 24) && (channel_cnt == 1) ) { //24  bit
		dac_data_cnt = wav_data_cnt / 3;
		wav_data8 = (uint8_t *)wav_data;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; ++i) {
				dac_data[i] =  (((wav_data8[2] & 0x7F) << 8) | wav_data8[1]) >> 4;
				wav_data8 += 3;
			}
		} else {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = 2047 + (((((wav_data8[2] & 0x7F) << 8) | wav_data8[1]) >> 4) - 2047) * volume /100;
				dac_data[i] =  BOUNDSMALL(0, 4095, data);
				wav_data8 += 3;
			}
		}
		return dac_data_cnt;
	}


	if ((bits_per_sample == 24) && (channel_cnt == 2) ) { //24  bit
		dac_data_cnt = wav_data_cnt / 6;
		wav_data8 = (uint8_t *)wav_data;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; ++i) {
				dac_data[i] =  (((wav_data8[2] & 0x7F) << 8) | wav_data8[1]) >> 4;
				wav_data8 += 6;
			}
		} else {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = 2047 + (((((wav_data8[2] & 0x7F) << 8) | wav_data8[1]) >> 4) - 2047) * volume /100;
				dac_data[i] =  BOUNDSMALL(0, 4095, data);
				wav_data8 += 6;
			}
		}
		return dac_data_cnt;
	}

	if ((bits_per_sample == 32) && (channel_cnt == 1)) { //32  bit
		dac_data_cnt = wav_data_cnt / 4;
		wav_data32 = (int32_t *)wav_data;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = 2047 + (wav_data32[i] >> 20);
				dac_data[i] = BOUNDSMALL(0, 4095, data);;
			}
		} else {
			for (i = 0; i < dac_data_cnt; ++i) {
				data =  2047 + (wav_data32[i] >> 20) * volume /100;;
				dac_data[i] = BOUNDSMALL(0, 4095, data);;
			}
		}
		return dac_data_cnt;
	}

	if ((bits_per_sample == 32) && (channel_cnt == 2)) { //32  bit
		dac_data_cnt = wav_data_cnt / 8;
		wav_data32 = (int32_t *)wav_data;
		if (volume == 100) {
			for (i = 0; i < dac_data_cnt; ++i) {
				data = 2047 + (*wav_data32 >> 20);
				dac_data[i] = BOUNDSMALL(0, 4095, data);;
				wav_data32 += 8;
			}
		} else {
			for (i = 0; i < dac_data_cnt; ++i) {
				data =  2047 + (*wav_data32 >> 20) * volume /100;;
				dac_data[i] = BOUNDSMALL(0, 4095, data);;
				wav_data32 += 8;
			}
		}
		return dac_data_cnt;
	}

	return 0;
}

//читает из файла WAV в dacdata, возвращвет количечтво точек
uint16_t wavfile2dacdata(FIL * file, uint16_t * dacdata, uint16_t dacdata_len, uint8_t bits, uint8_t chan, uint16_t volume)
{
	uint8_t * wavdata;
	FRESULT fr;
	UINT br;
	uint16_t wavdata_len;
	uint16_t res;

	if (bits == 32) wavdata_len = dacdata_len * 4;
	else if (bits == 24) wavdata_len = dacdata_len * 3;
	else if (bits == 16) wavdata_len = dacdata_len * 2;
	else if (bits == 8) wavdata_len = dacdata_len;
	else return 0;

	if (chan == 2) wavdata_len *= 2;


    wavdata = (uint8_t *)malloc(wavdata_len);

	fr = f_read(file, wavdata, wavdata_len,  &br);

	if ( (fr == FR_OK) && br) res = wavdata2dacdata(wavdata, br, dacdata, bits, chan, volume);
	else res = 0;

	free(wavdata);

	return res;
}



//Из WAV данных в PCM данные, возращает pcm_data_cnt
uint16_t wavdata2pcmdata2(uint8_t * wav_data, uint16_t wav_data_cnt, int16_t * pcm_data,  uint8_t bits_per_sample, uint8_t channel_cnt, uint16_t volume)
{
	uint16_t i, pcm_data_cnt;
	int16_t * wav_data16;
    int32_t data;


	if ( (bits_per_sample == 8) && (channel_cnt == 1) ) { //8 бит моно  OK
		pcm_data_cnt = wav_data_cnt;
		if (volume == 100) {
			for (i = 0; i < pcm_data_cnt; ++i) {
				pcm_data[i] = (int32_t)(wav_data[i] << 8) - 32767;
			}
		} else {
			for (i = 0; i < pcm_data_cnt; ++i) {
                                data = ((int32_t)(wav_data[i] << 8) - 32767) * volume / 100;
				pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
			}
		}
		return pcm_data_cnt;
	}

	if ( (bits_per_sample == 8) && (channel_cnt == 2) ) { //8 бит стерео OK
		pcm_data_cnt = wav_data_cnt / 2;
		if (volume == 100) {
			for (i = 0; i < pcm_data_cnt; ++i) {
				pcm_data[i] = (int32_t)(wav_data[i * 2] << 8) - 32767;
			}
		} else {
			for (i = 0; i < pcm_data_cnt; ++i) {
				data = ((int32_t)(wav_data[i * 2] << 8) - 32767) * volume / 100;
				pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
			}

		}
		return pcm_data_cnt;
	}


	if ( (bits_per_sample == 16) && (channel_cnt == 1) ) { //16 бит моно OK
		pcm_data_cnt = wav_data_cnt / 2;
		wav_data16 = (int16_t *)wav_data;
		if (volume == 100) {
			for (i = 0; i < pcm_data_cnt; ++i) {
				pcm_data[i] = wav_data16[i];
			}
		} else {
			for (i = 0; i < pcm_data_cnt; ++i) {
				data = wav_data16[i] * volume / 100;
				pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
			}

		}
		return pcm_data_cnt;
	}

	if ( (bits_per_sample == 16) && (channel_cnt == 2) ) { //16 бит стерео
		wav_data16 = (int16_t *)wav_data;
		pcm_data_cnt = wav_data_cnt / 4;
		if (volume == 100) {
			for (i = 0; i < pcm_data_cnt; i++) {
        			pcm_data[i] = wav_data16[i * 2];
			}
		} else {
			for (i = 0; i < pcm_data_cnt; i++) {
				data = wav_data16[i * 2] * volume / 100;
				pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
			}

		}
		return pcm_data_cnt;
	}

	return 0;
}

//читает из файла WAV в pcmdata, возвращвет количечтво точек
uint16_t wavfile2pcmdata2(FIL * file, int16_t * pcmdata, uint16_t pcmdata_len, uint8_t bits, uint8_t chan, uint16_t volume)
{
	uint8_t * wavdata;
	FRESULT fr;
	UINT br;
	uint16_t wavdata_len;
	uint16_t res;


	if ( (bits == 16) && (chan == 2) ) wavdata_len = pcmdata_len * 4;
	else if ( ( (bits == 8) && (chan == 2) ) || ( (bits == 16) && (chan == 1) ) ) wavdata_len = pcmdata_len * 2;
	else wavdata_len = pcmdata_len;


    wavdata = (uint8_t *)malloc(wavdata_len);

	fr = f_read(file, wavdata, wavdata_len,  &br);

	if ( (fr == FR_OK) && br) res = wavdata2pcmdata2(wavdata, br, pcmdata, bits, chan, volume);
	else res = 0;

    free(wavdata);

	return res;
}

/*

//читает из файла WAV в pcmdata, возвращвет количечтво точек
uint16_t wavfile2pcmdata(FIL * file, int16_t * pcmdata, uint16_t pcmdata_len, uint8_t bits, uint8_t chan, uint16_t volume)
{
	uint16_t res;

	res = wavfile2dacdata(file, (uint16_t *)pcmdata, pcmdata_len, bits, chan, 100);

	if (res) dacdata2pcmdata((uint16_t *)pcmdata, pcmdata, res, volume);

	return res;
}

*/

uint16_t wav2pcm16(uint8_t * wav_data, uint16_t wav_data_cnt, int16_t * pcm_data, uint16_t pcm_data_len, uint8_t bits_per_sample)
{
	uint16_t i, pcm_data_cnt;
	int16_t * wav_data16;
	int32_t * wav_data32;
	uint8_t * wav_data8;

	if (bits_per_sample == 8) { //8 bit
		pcm_data_cnt = MIN(wav_data_cnt, pcm_data_len);
		for (i = 0; i < pcm_data_cnt; ++i) {
			pcm_data[i] = (int32_t)(wav_data[i] << 8) - 32767;
		}
		return pcm_data_cnt;
	}


	if (bits_per_sample == 16) { //16  bit
		pcm_data_cnt = MIN(wav_data_cnt / 2, pcm_data_len);
		wav_data16 = (int16_t *)wav_data;
		for (i = 0; i < pcm_data_cnt; ++i) {
			pcm_data[i] = wav_data16[i];
		}
		return pcm_data_cnt;
	}

	if (bits_per_sample == 24) { //24  bit
		pcm_data_cnt = MIN(wav_data_cnt / 3, pcm_data_len);
		wav_data8 = (uint8_t *)wav_data;
		for (i = 0; i < pcm_data_cnt; ++i) {
                        pcm_data[i] =  (wav_data8[2] << 8) | wav_data8[1];
                        wav_data8 += 3;
		}
		return pcm_data_cnt;
	}

	if (bits_per_sample == 32) { //32  bit
		pcm_data_cnt = MIN(wav_data_cnt / 4, pcm_data_len);
		wav_data32 = (int32_t *)wav_data;
		for (i = 0; i < pcm_data_cnt; ++i) {
			pcm_data[i] = wav_data32[i] >> 16;
		}
		return pcm_data_cnt;
	}

	return 0;

}

uint16_t wavfile2pcm16(FIL * file, int16_t * pcm_data, uint16_t pcm_data_len, uint8_t bits)
{
	uint8_t * wavdata;
	FRESULT fr;
	UINT br;
	uint16_t wavdata_size;
	uint16_t res;


	if (bits == 32) wavdata_size = pcm_data_len * 4;
	else if (bits == 24) wavdata_size = pcm_data_len * 3;
	else if (bits == 16) wavdata_size = pcm_data_len * 2;
	else if (bits == 8) wavdata_size = pcm_data_len;
	else return 0;


    wavdata = (uint8_t *)malloc(wavdata_size);

	fr = f_read(file, wavdata, wavdata_size,  &br);

	if ( (fr == FR_OK) && br) res = wav2pcm16(wavdata, br, pcm_data, pcm_data_len, bits);
	else res = 0;

	free(wavdata);

	return res;
}

/*
//читает из файла WAV в alawdata, возвращвет количечтво точек
uint16_t wavfile2alawdata(FIL * file, int8_t * alawdata, uint16_t alawdata_len, uint8_t bits, uint8_t chan, uint16_t volume)
{
	int16_t * pcmdata;
	uint16_t res, i;

	pcmdata = (int16_t *)malloc(alawdata_len * 2);

	res = wavfile2pcmdata(file, pcmdata, alawdata_len, bits, chan, volume);

	for (i = 0; i < res; ++i) {
		alawdata[i] = ALaw_Encode(pcmdata[i]);
	}

	free(pcmdata);

	return res;
}
*/


const char * waverr2str(uint8_t err)
{
	switch (err) {
		case WAV_OK: return "no error";
		case WAV_ERROR_OPEN_PARAMS: return "error open params";
		case WAV_ERROR_OPEN_FILE: return "error open file";
		case WAV_ERROR_READ_HEADER: return "error read header";
		case WAV_ERROR_HEADER_FORMAT: return "error header format";
		case WAV_ERROR_HEADER_PARAMS: return "error header params";
		case WAV_ERROR_READ_CHUNK: return "error read chunk";
		default: return "unknow";
	}
}

uint8_t wavopenfile(const char * wav_file_name, FIL * a_file, wav_header_t * a_file_hdr)
{
	UINT br;
	FRESULT fr;
	char b;
	long id_size;

	f_close(a_file);

	if (!strlen(wav_file_name)) return WAV_ERROR_OPEN_PARAMS;

	if ( f_open(a_file, wav_file_name, FA_READ) != FR_OK) return WAV_ERROR_OPEN_FILE;

	if ( f_read(a_file, (void *)a_file_hdr, sizeof(wav_header_t),  &br) != FR_OK) {
		f_close(a_file);
//#ifndef USE_FATFS_IMITATION
//		ulog_fmt("Audio: error WAV file %s"CRLF, wav_file_name);
//#endif
		return WAV_ERROR_READ_HEADER;
	}

	if (br != sizeof(wav_header_t) || (memcmp(a_file_hdr->riff, "RIFF", 4) != 0) || (memcmp(a_file_hdr->wave, "WAVE", 4) != 0) /*|| (memcmp(a_file_hdr->id, "data", 4) != 0)*/ ) {
		f_close(a_file);
//#ifndef USE_FATFS_IMITATION
//		ulog_fmt("Audio: error WAV HEADER, file %s"CRLF, wav_file_name);
//#endif
		return WAV_ERROR_HEADER_FORMAT;
	}

	if ( ((a_file_hdr->bits_per_sample != 8) && (a_file_hdr->bits_per_sample != 16) && (a_file_hdr->bits_per_sample != 24) && (a_file_hdr->bits_per_sample != 32)) || ((a_file_hdr->channel != 1) && (a_file_hdr->channel != 2)) ) {
		f_close(a_file);
//#ifndef USE_FATFS_IMITATION
//		ulog_fmt("Audio: error WAV HEADER data, file %s"CRLF, wav_file_name);
//#endif
		return WAV_ERROR_HEADER_PARAMS;
	}


	id_size = 0;
	while (true) {
	    fr = f_read(a_file, &b, 1,  &br);
	    if (fr != FR_OK) break;
	    if (b != 'd') continue;
	    fr = f_read(a_file, &b, 1,  &br);
	    if (fr != FR_OK) break;
	    if (b != 'a') continue;
	    fr = f_read(a_file, &b, 1,  &br);
	    if (fr != FR_OK) break;
	    if (b != 't') continue;
	    fr = f_read(a_file, &b, 1,  &br);
	    if (fr != FR_OK) break;
	    if (b != 'a') continue;
	    fr = f_read(a_file, &id_size, 4,  &br);
	    break;
	}

	if (id_size == 0) {
		f_close(a_file);
//#ifndef USE_FATFS_IMITATION
//		ulog_fmt("Audio: error CHUNK size, file %s"CRLF, wav_file_name);
//#endif
		return  WAV_ERROR_READ_CHUNK;
	}

	return WAV_OK;
}

bool wav_open_file(const char * wav_file_name, FIL * a_file, wav_header_t * a_file_hdr)
{
	return (wavopenfile(wav_file_name, a_file, a_file_hdr) == WAV_OK);
}
