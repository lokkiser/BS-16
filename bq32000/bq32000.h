#ifndef RTC_BQ32000_H
#define RTC_BQ32000_H

#include "main.h"

#define RTC_I2C_ADDRESS 0x68

#define RTC_REG_TIME_SEC                0x00
#define RTC_REG_TIME_MIN                0x01
#define RTC_REG_TIME_HOUR               0x02
#define RTC_REG_TIME_DAY_OF_THE_WEEK    0x03
#define RTC_REG_TIME_DATE_DAY           0x04
#define RTC_REG_TIME_DATE_MONTH         0x05
#define RTC_REG_TIME_DATE_YEAR          0x06
#define RTC_REG_CONFIG1                 0x07
#define RTC_REG_TRICKLE_CHARGE2         0x08
#define RTC_REG_CONFIG2                 0x09
#define RTC_REG_SF_KEY1                 0x20
#define RTC_REG_SF_KEY2                 0x21
#define RTC_REG_SFR                     0x22

typedef struct
{
    uint8_t seconds; // 00-59 in BCD
    uint8_t minutes; // 00-59 in BCD
    uint8_t hours;   // 00-23 in BCD
    uint8_t dayOfWeek;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} RTC_Time;

extern RTC_Time globalRTC_Time; // Declare global variable

void RTC_BQ32000_Init(I2C_HandleTypeDef* hi2c);
HAL_StatusTypeDef RTC_BQ32000_ReadRegister(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t* data);
HAL_StatusTypeDef RTC_BQ32000_WriteRegister(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t data);
HAL_StatusTypeDef RTC_BQ32000_ReadTime(I2C_HandleTypeDef* hi2c, RTC_Time* rtcTime);
HAL_StatusTypeDef RTC_BQ32000_SetTime(I2C_HandleTypeDef* hi2c, RTC_Time* rtcTime);

#endif /* RTC_BQ32000_H */
