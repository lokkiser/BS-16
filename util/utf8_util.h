/*
 * utf8_util.h
 *
 *  Created on: 26 янв. 2022 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_UTF8_UTIL_H_
#define UTIL_UTF8_UTIL_H_


#include <string.h>

void UTF8to1251(char * utf8, char * buf, size_t buf_size);

void UTF8toISO8859_5(char * utf8, char * buf, size_t buf_size);

void CP866ToUTF8(char *utf_string, char *cp_string);

void UTF8ToCP866(char *dst, unsigned int dst_size, const char *src);

#endif /* UTIL_UTF8_UTIL_H_ */
