#include "bq32000.h"

RTC_Time globalRTC_Time; // Define global variable

void RTC_BQ32000_Init(I2C_HandleTypeDef* hi2c) {
    // Initialization code if needed
}

HAL_StatusTypeDef RTC_BQ32000_ReadRegister(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t* data) {
    return HAL_I2C_Mem_Read(hi2c, RTC_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef RTC_BQ32000_WriteRegister(I2C_HandleTypeDef* hi2c, uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(hi2c, RTC_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef RTC_BQ32000_ReadTime(I2C_HandleTypeDef* hi2c, RTC_Time* rtcTime) {
    // Read the time registers
    uint8_t data[7];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, RTC_I2C_ADDRESS << 1, RTC_REG_TIME_SEC, I2C_MEMADD_SIZE_8BIT, data, 7, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        rtcTime->seconds = ((data[0] & 0xF0) >> 4) * 10 + (data[0] & 0x0F);
        rtcTime->minutes = ((data[1] & 0xF0) >> 4) * 10 + (data[1] & 0x0F);
        rtcTime->hours = ((data[2] & 0xF0) >> 4) * 10 + (data[2] & 0x0F);
        rtcTime->dayOfWeek = data[3];
        rtcTime->day = ((data[4] & 0xF0) >> 4) * 10 + (data[4] & 0x0F);
        rtcTime->month = ((data[5] & 0xF0) >> 4) * 10 + (data[5] & 0x0F);
        rtcTime->year = ((data[6] & 0xF0) >> 4) * 10 + (data[6] & 0x0F);
        // Update the global variable
        globalRTC_Time = *rtcTime;
    }

    return status;
}

HAL_StatusTypeDef RTC_BQ32000_SetTime(I2C_HandleTypeDef* hi2c, RTC_Time* rtcTime) {
    // Write the time registers
    uint8_t data[7] = {
        ((rtcTime->seconds / 10) << 4) | (rtcTime->seconds % 10),
        ((rtcTime->minutes / 10) << 4) | (rtcTime->minutes % 10),
        ((rtcTime->hours / 10) << 4) | (rtcTime->hours % 10),
        rtcTime->dayOfWeek,
        ((rtcTime->day / 10) << 4) | (rtcTime->day % 10),
        ((rtcTime->month / 10) << 4) | (rtcTime->month % 10),
        ((rtcTime->year / 10) << 4) | (rtcTime->year % 10),
    };

    return HAL_I2C_Mem_Write(hi2c, RTC_I2C_ADDRESS << 1, RTC_REG_TIME_SEC, I2C_MEMADD_SIZE_8BIT, data, 7, HAL_MAX_DELAY);
}
