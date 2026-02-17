/*
 * audio_util.c
 *
 *  Created on: 8 июн. 2020 г.
 *      Author: zugan
 */

#include "audio_util.h"
#include "def_util.h"


FIL audio_file = {0};

/*
//их данных PCM в DAC
void pcmdata2dacdata(int16_t * pcm_data, uint16_t * dac_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int16_t v;

	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			v = pcm_data[i] + 2047;
			dac_data[i] = BOUNDSMALL(0, 4095, v);
		}
	} else {
		for (i = 0; i < data_len; ++i) {
			v = (pcm_data[i] + 2047)  * volume / 100;
			dac_data[i] = BOUNDSMALL(0, 4095, v);
		}

	}
}

//их данных DAC в PCM
void dacdata2pcmdata(uint16_t * dac_data, int16_t * pcm_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int32_t data;

	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			pcm_data[i] = (int16_t)dac_data[i] - 2047;
		}
	} else {
		for (i = 0; i < data_len; ++i) {
			data = ((int16_t)dac_data[i] - 2047) * volume / 100;
			pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
		}

	}
}
*/


