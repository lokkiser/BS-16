/*
 * utf8_util.c
 *
 *  Created on: 26 янв. 2022 г.
 *      Author: Rafael Boltachev
 */

#include "utf8_util.h"


// перекодирует все однобайтовые символы, а из двубайтовых только русские буквы
void UTF8to1251(char * utf8, char * buf, size_t buf_size)
{
	size_t i, cnt, utf8_len;
	unsigned char b1, b2, * uutf8, * ubuf;

	cnt = 0;
        utf8_len = strlen(utf8);
	uutf8 = (unsigned char *)utf8;
	ubuf = (unsigned char *)buf;

    for (i = 0; i < utf8_len; ++i) {
        if (cnt >= buf_size - 1) break;
        b1 = uutf8[i];
        if ((b1 >> 5) == 6)  {
            b2 = utf8[i + 1];
            if ((b1 == 208) && (b2 == 129)) //Ё
            {
            	ubuf[cnt] = b2 + 39;
                cnt++;
            }
            else if ((b1 == 209) && (b2 == 145)) //ё
            {
            	ubuf[cnt] = b2 + 39;
                cnt++;
            }
            else if ((b1 == 208) && (b2 >= 144 && b2 <= 191)) //А-Яа-п
            {
            	ubuf[cnt] = b2 + 48;
                cnt++;
            }
            else if ((b1 == 209) && (b2 >= 128 && b2 <= 143)) //р-я
            {
            	ubuf[cnt] = b2 + 112;
                cnt++;
            }
            else if ((b1 == 194) && (b2 == 176)) //градус
            {
            	ubuf[cnt] = b2;
                cnt++;
            }
            ++i;
        }  else if ((b1 >> 7) == 0)  {
            ubuf[cnt] =  b1;
            cnt++;
        }
    }
    buf[cnt] = '\0';
}

void UTF8toISO8859_5(char * utf8, char * buf, size_t buf_size)
{
	size_t i, cnt, utf8_len;
	unsigned char b1, b2, * uutf8, * ubuf;

	cnt = 0;
	utf8_len = strlen(utf8);
	uutf8 = (unsigned char *)utf8;
	ubuf = (unsigned char *)buf;

    for (i = 0; i < utf8_len; ++i) {
        if (cnt + 1 >= buf_size) break;
        b1 = uutf8[i];
        if ((b1 >> 5) == 6)  {
            b2 = utf8[i + 1];
            if ((b1 == 208) && (b2 == 129)) //Ё
            {
            	ubuf[cnt] = 161;
                cnt++;
            }
            else if ((b1 == 209) && (b2 == 145)) //ё
            {
            	ubuf[cnt] = 241;
                cnt++;
            }
            else if ((b1 == 208) && (b2 >= 144 && b2 <= 191)) //А-Яа-п
            {
            	ubuf[cnt] = b2 + 32;
                cnt++;
            }
            else if ((b1 == 209) && (b2 >= 128 && b2 <= 143)) //р-я
            {
            	ubuf[cnt] = b2 + 96;
                cnt++;
            }
            ++i;
        }  else if ((b1 >> 7) == 0)  {
            ubuf[cnt] =  b1;
            cnt++;
        }
    }
    buf[cnt] = '\0';
}



void CP866ToUTF8(char *utf_string, char *cp_string)
{
    int i, j;
	union	{
		unsigned short int code_utf;
		char ch_for_buff[2];
	} to_utf;

	for(i=0,j=0; i<strlen(cp_string); i++,j+=2)
	{
		if((unsigned char)cp_string[i]>=0x80 && (unsigned char)cp_string[i]<=0xAF)  {
			to_utf.code_utf = 0xD010+(unsigned char)cp_string[i];
			utf_string[j]=to_utf.ch_for_buff[1];
			utf_string[j+1]=to_utf.ch_for_buff[0];
		}
		else if((unsigned char)cp_string[i]>=0xE0 && (unsigned char)cp_string[i]<=0xEF)	{
			to_utf.code_utf = 0xD0A0+(unsigned char)cp_string[i];
			utf_string[j]=to_utf.ch_for_buff[1];
			utf_string[j+1]=to_utf.ch_for_buff[0];
		}
		else if((unsigned char)cp_string[i]==0xF0)  {
			to_utf.code_utf = 0xD081;
			utf_string[j]=to_utf.ch_for_buff[1];
			utf_string[j+1]=to_utf.ch_for_buff[0];
		}
		else if((unsigned char)cp_string[i]==0xF1)  {
			to_utf.code_utf = 0xD191;
			utf_string[j]=to_utf.ch_for_buff[1];
			utf_string[j+1]=to_utf.ch_for_buff[0];
		}
		else if((unsigned short int)cp_string[i]>=0x30 && (unsigned short int)cp_string[i]<=0x39)  {
			to_utf.code_utf = (unsigned short int)cp_string[i];
			utf_string[j--]=to_utf.ch_for_buff[0];
		}
		else {
			utf_string[j--]= cp_string[i];
		}
	}
    utf_string[j] = '\0';
}


/**
* @brief Переводим из UTF-8 в CP866
* @param dst - по какому адресу пишем
* @param src - откуда берём строку, до первого '\0'
*/
void UTF8ToCP866(char *dst, unsigned int dst_size, const char *src)
{
  unsigned int i =0, j =0;
  unsigned char cur, conv;
  //-- Пробегаемся по всем символам, учитываем отличия между таблицами
  for (i=0; src[i]!=0; ++i) {
	if (j >= dst_size) {
		j--;
		break;
	}
    cur = src[i];
    if ( cur==208 || cur==209 ) {
      conv = src[++i];
      if ( cur==208 ) {
        if ( conv>=144 && conv<192 ) { conv -= 16; }
      } else {
        if ( conv>=128 && conv<144 ) { conv += 96; }
      }
      dst[j++] = conv;
    } else {
      dst[j++] = cur;
    }
  }
  dst[j] = 0;
}
