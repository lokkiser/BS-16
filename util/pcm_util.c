/*
 * pcm_util.c
 *
 *  Created on: 16 февр. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "pcm_util.h"
#include "def_util.h"


void pcm_set_value(int16_t *pcm_data, uint16_t pcm_data_len, int16_t value)
{
	uint16_t i;

	for (i = 0; i < pcm_data_len; ++i) {
		pcm_data[i] = value;
	}
}


void pcm_set_volume(int16_t *dst_pcm_data, int16_t *src_pcm_data, uint16_t pcm_data_len, uint16_t volume)
{
	uint16_t i;
	int32_t data;


	for (i = 0; i < pcm_data_len; ++i) {
		data = (int32_t)src_pcm_data[i] * volume / 100;
		dst_pcm_data[i] = BOUNDSMALL(-32768, 32767, data);
	}

}

void pcm_set_shift_right(int16_t *dst_pcm_data, int16_t *src_pcm_data, uint16_t pcm_data_len, uint8_t bits)
{
	uint16_t i;


	for (i = 0; i < pcm_data_len; ++i) {
		dst_pcm_data[i] = src_pcm_data[i] >> bits;
	}
}


void pcm_mono2dac12(int16_t * pcm_data, uint16_t * dac_data, uint16_t data_len)
{
	uint16_t i;

	for (i = 0; i < data_len; ++i) {
		dac_data[i] = ((int32_t)pcm_data[i] + 32768) >> 4;
	}
}

void pcm_adc12mono2pcm(uint16_t * adc_data, int16_t * pcm_data, uint16_t data_len)
{
	uint16_t i;

	for (i = 0; i < data_len; ++i) {
		pcm_data[i] = ((int16_t)adc_data[i] - 2047) << 4;
	}
}


uint16_t pcm_leftright2stereo(int16_t * pcm_left, uint16_t pcm_left_cnt, int16_t * pcm_right, uint16_t pcm_right_cnt, int16_t * pcm_stereo)
{
	uint16_t i, j;

	j = 0;
	if (pcm_left_cnt && pcm_right_cnt) {

		if (pcm_left_cnt == pcm_right_cnt) {
        		for (i = 0; i < pcm_left_cnt; i++) {
	        		pcm_stereo[j] = pcm_left[i];
		        	j++;
			        pcm_stereo[j] = pcm_right[i];
        			j++;
	        	}
		}
		else if (pcm_left_cnt > pcm_right_cnt) {
        		for (i = 0; i < pcm_left_cnt; i++) {
	        		pcm_stereo[j] = pcm_left[i];
		        	j++;
			        if (i < pcm_right_cnt) pcm_stereo[j] = pcm_right[i];
			        else pcm_stereo[j] = 0;
        			j++;
	        	}
		}
		else {
        		for (i = 0; i < pcm_right_cnt; i++) {
	        		if (i < pcm_left_cnt) pcm_stereo[j] = pcm_left[i];
	        		else pcm_stereo[j] = 0;
		        	j++;
			        pcm_stereo[j] = pcm_right[i];
        			j++;
	        	}
		}



	}
	else if (pcm_left_cnt) {
		for (i = 0; i < pcm_left_cnt; i++) {
			pcm_stereo[j] = pcm_left[i];
			j++;
			pcm_stereo[j] = 0;
			j++;
		}
	}
	else if (pcm_right_cnt) {
		for (i = 0; i < pcm_right_cnt; i++) {
			pcm_stereo[j] = 0;
			j++;
			pcm_stereo[j] = pcm_right[i];
			j++;
		}
	}



	return j;
}


uint16_t pcm_stereo2leftright(int16_t * pcm_stereo, uint16_t pcm_stereo_cnt, int16_t * pcm_left, int16_t * pcm_right)
{
	uint16_t i, j;
	uint16_t res;


	res = pcm_stereo_cnt / 2;
	j = 0;

	for (i = 0; i < res; i++) {
		pcm_left[i] = pcm_stereo[j];
		j++;
		pcm_right[i] = pcm_stereo[j];
		j++;
	}

	return res;
}


uint16_t pcm_stereo2mono(int16_t * pcm_stereo, uint16_t pcm_stereo_cnt, int16_t * pcm_mono)
{
	uint16_t i, j, pcm_left, pcm_right;
	uint16_t res;


	res = pcm_stereo_cnt / 2;
	j = 0;

	for (i = 0; i < res; i++) {
		pcm_left = pcm_stereo[j];
		j++;
		pcm_right = pcm_stereo[j];
		j++;
		pcm_mono[i] = (pcm_left + pcm_right) / 2;
	}

	return res;
}

uint16_t pcm_mono2stereo(int16_t * pcm_mono, uint16_t pcm_mono_cnt, int16_t * pcm_stereo)
{
        uint16_t i, j;

        j = 0;
	for (i = 0; i < pcm_mono_cnt; i++) {
            pcm_stereo[j] = pcm_mono[i];
            j++;
            pcm_stereo[j] = pcm_mono[i];
            j++;
        }
        return j;
}


uint16_t pcm_change_frequency(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels)
{
  uint16_t dst_cnt, i, j;


  if (!src_cnt) return 0;

  dst_cnt = src_cnt * dst_frquency / src_frquency;


  if (channels == 2) {
        for (i = 0; i < dst_cnt - 1; i += 2 ) {
                j = i * src_cnt / dst_cnt;

                if (ODD(j)) j++;

                dst[i] = src[j];
                dst[i + 1] = src[j + 1];
        }

  } else {
        for (i = 0; i < dst_cnt; i++) {
                j = i * src_cnt / dst_cnt;
                dst[i] = src[j];
        }
  }

  return dst_cnt;
}

uint16_t pcm_change_frequency2(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels)
{
  uint16_t dst_cnt, i, j, maxj;
  //int c, m;


  if (!src_cnt) return 0;

  dst_cnt = src_cnt * dst_frquency / src_frquency;


  if (channels == 2) {

        maxj = src_cnt - 3;
        for (i = 0; i < dst_cnt - 1; i += 2 ) {
                j = i * src_cnt / dst_cnt;

                if (ODD(j)) j++;

                if (j < maxj) {
                  dst[i] = (src[j] + src[j + 2]) / 2;  //left
                  dst[i + 1] = (src[j + 1] + src[j + 3]) / 2; //right

                } else {
                  dst[i] = src[j];  //left
                  dst[i + 1] = src[j + 1]; //right
                }
        }

  } else {
        maxj = src_cnt - 1;

        for (i = 0; i < dst_cnt; i++) {

        	j = i * src_cnt / dst_cnt;

                if (j < maxj) {
                	//dst[i] = (src[j] + src[j + 1]) >> 1;
                        dst[i] = (src[j] + src[j + 1]) / 2;
                }
                else {
                  dst[i] = (src[j] + src[j - 1]) / 2;
                }
/*
                c = i * src_cnt  * 100 / dst_cnt;
                j = c  / 100;
                m = c - j * 100;

                if (j < maxj) {
                	dst[i] = src[j] + m * (src[j + 1] - src[j]) / 99;
                }
                else dst[i] = src[j];
*/
        }
  }

  return dst_cnt;
}




uint16_t pcm_change_frequency_low_mono(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels)
{
  uint16_t i, j, k, n;
  int32_t sum;
  uint32_t new_frquency;

  if (!src_cnt) return 0;

  n = src_frquency / dst_frquency;

  if (n < 2) return pcm_change_frequency(dst, dst_frquency, src, src_cnt, src_frquency, 1);

  j = 0;
  k = 0;
  sum = 0;
  for (i = 0; i < src_cnt; i += n) {
    sum = src[i];
    for (j = 1; j < n; j++) {
       sum += src[i + j];
    }
    dst[k] = sum / n;
    k++;
  }


  new_frquency = src_frquency / n ;

  if (dst_frquency ==  new_frquency) return k;




  return pcm_change_frequency(dst, dst_frquency, dst, k, new_frquency, 1);
}


uint16_t pcm_change_frequency_mono(int16_t *dst_mono, uint32_t dst_frquency, int16_t *src_mono, uint16_t src_cnt, uint32_t src_frquency)
{
  uint16_t dst_cnt, i, j;
  float v, mul;

  dst_cnt = src_cnt * dst_frquency / src_frquency;

  for (i = 0; i < dst_cnt; i++) {
    v = (float)(i * src_cnt) / dst_cnt;
    j = (uint16_t)v;
    mul = v - (float)j;
    if (j < src_cnt - 1) {
      dst_mono[i] = src_mono[j] + (src_mono[j + 1] - src_mono[j]) * mul;
    }
    else {
      dst_mono[i] = src_mono[j];
    }

  }

  return dst_cnt;
}


uint16_t pcm_change_frequency3(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency, uint8_t channels)
{
	 uint16_t dst_cnt, i, j;
	 float v, mul;

	 if (!src_cnt) return 0;

	 dst_cnt = src_cnt * dst_frquency / src_frquency;



	  if (channels == 2) {
	        for (i = 0; i < dst_cnt - 1; i += 2 ) {


	                v = (float)(i * src_cnt) / dst_cnt;
	    		    j = (uint16_t)v;
	    		    mul = v - (float)j;

                    if (ODD(j)) {
                    	 j++;
                    }


  	  		        dst[i] = src[j] + (src[j + 2] - src[j]) * mul;
  	  		        dst[i + 1] = src[j + 1] + (src[j + 3] - src[j + 1]) * mul;
	        }

	  } else {
		  for (i = 0; i < dst_cnt; i++) {
		    v = (float)(i * src_cnt) / dst_cnt;
		    j = (uint16_t)v;
		    mul = v - (float)j;
		    if (j < src_cnt - 1) {
		      dst[i] = src[j] + (src[j + 1] - src[j]) * mul;
		    }
		    else {
		      dst[i] = src[j];
		    }
		  }
	  }

	  return dst_cnt;

}


uint16_t pcm_change_frequency_stereo(int16_t *dst, uint32_t dst_frquency, int16_t *src, uint16_t src_cnt, uint32_t src_frquency)
{
  int16_t pcm16_left_1[3000] = {0}, pcm16_left_2[3000] = {0};
  int16_t pcm16_right_1[3000] = {0}, pcm16_right_2[3000] = {0};
  uint16_t rc, rc_left, rc_right;

  rc = pcm_stereo2leftright(src, src_cnt, pcm16_left_1, pcm16_right_1);

  rc_left = pcm_change_frequency_mono(pcm16_left_2, dst_frquency, pcm16_left_1, rc, src_frquency);
  rc_right = pcm_change_frequency_mono(pcm16_right_2, dst_frquency, pcm16_right_1, rc, src_frquency);

  rc = pcm_leftright2stereo(pcm16_left_2, rc_left, pcm16_right_2, rc_right, dst);

  return rc;
}


uint16_t pcm_mono_calc_power(int16_t *src, uint16_t src_cnt)
{
	uint16_t i, cnt;
	uint32_t power_sum;

	power_sum = 0;

	cnt = src_cnt - 1;
	for (i = 0; i < cnt; ++i) {
		if (src[i] > src[i + 1]) power_sum += src[i] - src[i + 1];
		else power_sum += src[i + 1] - src[i];
	}
	return power_sum /  cnt;
}

