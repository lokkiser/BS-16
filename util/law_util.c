/*
 * law_util.c
 *
 *  Created on: 6 дек. 2023 г.
 *      Author: Rafael Boltachev
 */

#include "law_util.h"
#include "def_util.h"
#include "g711_util.h"

// --- ALAW ---

//из ALaw в DAC12 данные
void alaw2dac(int8_t * alaw_data, uint16_t * dac_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int16_t data;


	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			data = 2047 + ALaw_Decode(alaw_data[i]) / 2;
			dac_data[i] = BOUNDSMALL(0, 4095, data);
		}
	}
	else {
		for (i = 0; i < data_len; ++i) {
			data = 2047 + (ALaw_Decode(alaw_data[i]) / 2) * volume / 100;
			dac_data[i] = BOUNDSMALL(0, 4095, data);
		}
	}
}



//из АЦП12 в ALaw  данные
void adc2alaw(uint16_t * adc_data, int8_t * alaw_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;

	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			alaw_data[i] = ALaw_Encode((int16_t)adc_data[i]  - 2047);
		}
	} else {
		for (i = 0; i < data_len; ++i) {
			alaw_data[i] = ALaw_Encode( ((int16_t)adc_data[i]  - 2047) * volume / 100 );
		}
	}

}


//из ALAW в PCM
void alaw2pcm(int8_t * alaw_data, int16_t * pcm_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int32_t data;


	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			pcm_data[i] = ALaw_Decode(alaw_data[i]) << 4;
		}
	}
	else {
		for (i = 0; i < data_len; ++i) {
			data = (ALaw_Decode(alaw_data[i]) << 4) * volume / 100;
			pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
		}
	}
}

//из PCM в ALAW
void pcm2alaw(int16_t * pcm_data, int8_t * alaw_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int32_t data;

	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			alaw_data[i] = ALaw_Encode(pcm_data[i] >> 4);
		}
	}
	else {
		for (i = 0; i < data_len; ++i) {
			data = ALaw_Encode(pcm_data[i] >> 4) * volume / 100;
			alaw_data[i] = BOUNDSMALL(-128, 127, data);
		}

	}

}


// --- ULAW ---


//из ULaw в DAC12 данные
void ulaw2dac(int8_t * ulaw_data, uint16_t * dac_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int16_t data;


	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			data = 2047 + MuLaw_Decode(ulaw_data[i]) / 4;
			dac_data[i] = BOUNDSMALL(0, 4095, data);
		}
	}
	else {
		for (i = 0; i < data_len; ++i) {
			data = 2047 + (MuLaw_Decode(ulaw_data[i]) / 4) * volume / 100;
			dac_data[i] = BOUNDSMALL(0, 4095, data);
		}
	}
}

//из АЦП12 в ULaw  данные
void adc2ulaw(uint16_t * adc_data, int8_t * ulaw_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;

	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			ulaw_data[i] = MuLaw_Encode((int16_t)adc_data[i]  - 2047);
		}
	} else {
		for (i = 0; i < data_len; ++i) {
			ulaw_data[i] = MuLaw_Encode( ((int16_t)adc_data[i]  - 2047) * volume / 100 );
		}
	}

}

//из ULAW в PCM
void ulaw2pcm(int8_t * ulaw_data, int16_t * pcm_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int32_t data;


	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			pcm_data[i] = MuLaw_Decode(ulaw_data[i]) << 3;
		}
	}
	else {
		for (i = 0; i < data_len; ++i) {
			data = (MuLaw_Decode(ulaw_data[i]) << 3) * volume / 100;
			pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
		}
	}
}

//из PCM в ULAW
void pcm2ulaw(int16_t * pcm_data, int8_t * ulaw_data, uint16_t data_len, uint16_t volume)
{
	uint16_t i;
	int32_t data;

	if (volume == 100) {
		for (i = 0; i < data_len; ++i) {
			ulaw_data[i] = MuLaw_Encode(pcm_data[i] >> 3);
		}
	}
	else {
		for (i = 0; i < data_len; ++i) {
			data = MuLaw_Encode(pcm_data[i] >> 3) * volume / 100;
			ulaw_data[i] = BOUNDSMALL(-128, 127, data);
		}

	}

}
