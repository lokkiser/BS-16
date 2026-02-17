/*
 * datetime_util.c
 *
 *  Created on: 19 янв. 2022 г.
 *      Author: Rafael Boltachev
 */
#include "datetime_util.h"
#include <stdio.h>
#include "str_util.h"

static char __date_time2str_buf[32];

char* date_time2str(date_time_t date_time, bool with_second)
{
	if (with_second) sprintf(__date_time2str_buf, "%04d-%02d-%02d %02d:%02d:%02d", date_time.year, date_time.month, date_time.date, date_time.hour, date_time.minute, date_time.second);
	else sprintf(__date_time2str_buf, "%04d-%02d-%02d %02d:%02d", date_time.year, date_time.month, date_time.date, date_time.hour, date_time.minute);
	return __date_time2str_buf;
}

char* date_2str(date_time_t date_time)
{
	snprintf(__date_time2str_buf, sizeof(__date_time2str_buf), "%04d-%02d-%02d", date_time.year, date_time.month, date_time.date);
	return __date_time2str_buf;
}

date_time_t str2date_time(char * str)
{
	date_time_t date_time;
	char *ptr;

	memset(&date_time, 0, sizeof(date_time));
	date_time.year = str2int(str);
	ptr = strright(str, strlen(str), "-");
	if (!ptr) return date_time;
	date_time.month = str2int(ptr);
	ptr = strright(ptr, strlen(ptr), "-");
	if (!ptr) return date_time;
	date_time.date = str2int(ptr);
	ptr = strright(ptr, strlen(ptr), " ");
	if (!ptr) return date_time;
	date_time.hour = str2int(ptr);
	ptr = strright(ptr, strlen(ptr), ":");
	if (!ptr) return date_time;
	date_time.minute = str2int(ptr);
	ptr = strright(ptr, strlen(ptr), ":");
	if (!ptr) return date_time;
	date_time.second = str2int(ptr);

	return date_time;
}

//примерное
uint64_t timestamp(date_time_t* date_time)
{
	return (date_time->year - 1970) * 31536000 + (date_time->month - 1) *  2678400 + (date_time->date - 1) * 86400 + date_time->hour * 3600 + date_time->minute * 60 + date_time->second;

}

