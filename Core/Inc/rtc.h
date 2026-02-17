/*
 * rtc.h
 *
 *  Created on: 7 мар. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef CORE_INC_RTC_H_
#define CORE_INC_RTC_H_

#include "main.h"
#include "bq32000.h"

#define RTC_I2C_HANDLE	&hi2c3


void rtc_init(void);
void rtcdatetime2str(RTC_Time *rtcTim, char * buf, uint16_t buf_size, bool with_second);
bool str2rtcdatetime(char * str, RTC_Time *rtcTim);
char * rtc_read_datetime_str(bool with_second);
void rtc_set_datetime_str(char * str);
char * rtc_read_time_str(bool with_second);


#endif /* CORE_INC_RTC_H_ */
