/*
 * datetime_util.h
 *
 *  Created on: 19 янв. 2022 г.
 *      Author: Rafael Boltachev
 *
 *      библиотека дата время
 */

#ifndef DATETIME_UTIL_H_
#define DATETIME_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "def_util.h"


typedef struct _date_time 
{
	unsigned short year;
	unsigned char month, date, hour, minute, second;
} date_time_t;

//date_time в строку
char* date_time2str(date_time_t date_time, bool with_second);
char* date_2str(date_time_t date_time);
//из строки в date_time
date_time_t str2date_time(char* str);
//тайм стамп
uint64_t timestamp(date_time_t* date_time);

#define YEAR_GOD(y) (y > 2000)
#define MONTH_GOD(m) ((m > 0) && (m < 13))
#define DATE_GOD(d) ((d > 0) && (d < 32))
#define HOUR_GOD(h) (h < 25)
#define MINUTE_GOD(m) (m < 60)
#define SECOND_GOD(s) (s < 60)

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DATETIME_UTIL_H_ */
