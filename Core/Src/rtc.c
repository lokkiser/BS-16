/*
 * rtc.c
 *
 *  Created on: 7 мар. 2025 г.
 *      Author: Rafael Boltachev
 */

#include "rtc.h"
#include <string.h>
#include <stdio.h>
#include "str_util.h"
#include "stm_util.h"


static char __rtc_read_datetime_str[22];


void rtc_init(void)
{
	  RTC_BQ32000_Init(RTC_I2C_HANDLE);
}


// RTC_Time в стринг
void rtcdatetime2str(RTC_Time* rtcTim, char* buf, uint16_t buf_size, bool with_second)
{
    if (with_second ) snprintf(buf, buf_size, "20%02d-%02d-%02dT%02d:%02d:%02d", rtcTim->year, rtcTim->month, rtcTim->day, rtcTim->hours, rtcTim->minutes, rtcTim->seconds);
    else snprintf(buf, buf_size, "20%02d-%02d-%02dT%02d:%02d", rtcTim->year, rtcTim->month, rtcTim->day, rtcTim->hours, rtcTim->minutes);
}

//стринг в RTC_Time
bool str2rtcdatetime(char* str, RTC_Time* rtcTime)
{
	char* ptr;
	int year;

	memset(rtcTime, 0, sizeof(RTC_Time));

	year = str2int(str);
	if (year < 2000) return false;
	rtcTime->year = year - 2000;

	ptr = strright(str, strlen(str), "-");
	if (!ptr) return false;
	rtcTime->month = str2int(ptr);

	ptr = strright(ptr, strlen(str),  "-");
	if (!ptr) return false;
	rtcTime->day = str2int(ptr);

	ptr = strright(ptr, strlen(str),  "T");
	if (ptr) rtcTime->hours = str2int(ptr);

	ptr = strright(ptr, strlen(str),  ":");
	if (ptr) rtcTime->minutes = str2int(ptr);

	ptr = strright(ptr, strlen(str),  ":");
	if (ptr) rtcTime->seconds = str2int(ptr);

	return true;
}


//Прочитать дату и время с RTC в стринг
char* rtc_read_datetime_str(bool with_second)
{
	RTC_Time rtcTime;


    RTC_BQ32000_ReadTime(RTC_I2C_HANDLE, &rtcTime);

    rtcdatetime2str(&rtcTime, __rtc_read_datetime_str, sizeof(__rtc_read_datetime_str), with_second);

	return __rtc_read_datetime_str;
}

//Установка RTC из строки
void rtc_set_datetime_str(char* str)
{
	RTC_Time rtcTime;

	if (!str2rtcdatetime(str, &rtcTime)) {
		ulog("rtc: error str2rtctime"CRLF);
		return;
	}

	ulog_fmt("rtc: set datetime  %d-%d-%d %d:%d:%d"CRLF, rtcTime.year, rtcTime.month, rtcTime.day, rtcTime.hours, rtcTime.minutes, rtcTime.seconds);
	RTC_BQ32000_SetTime(RTC_I2C_HANDLE, &rtcTime);
}


// RTC_Time в стринг (только время)
void rtctime2str(RTC_Time* rtcTim, char* buf, uint16_t buf_size, bool with_second)
{
    if (with_second ) snprintf(buf, buf_size, "%02d:%02d:%02d", rtcTim->hours, rtcTim->minutes, rtcTim->seconds);
    else snprintf(buf, buf_size, "%02d:%02d", rtcTim->hours, rtcTim->minutes);
}


//Прочитать время с RTC в стринг
char* rtc_read_time_str(bool with_second)
{
	RTC_Time rtcTime;


    RTC_BQ32000_ReadTime(RTC_I2C_HANDLE, &rtcTime);

    rtctime2str(&rtcTime, __rtc_read_datetime_str, sizeof(__rtc_read_datetime_str), with_second);

	return __rtc_read_datetime_str;
}

