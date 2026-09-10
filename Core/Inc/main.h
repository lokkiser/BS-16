/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "def_util.h"
#include "soft_timer.h"
#include "tim.h"

#define BASE_FREQUENCY 				 165888000
#define BASE_TIM_FREQUENCY 			 BASE_FREQUENCY
#define BASE_AUDIO_FREQUENCY 		 48000
#define HAL_ADC_INJECTED_ENABLED	 1
#define SOUND_DMA_BUFFER_SIZE 		 1024
#define BC16_LOADER_FILE_NAME 		"bc16_loader.bin"
#define BC16_LOADER_ADDR			(0x08060000)
#define BC16_UPGRADE_FILE_NAME	    "bc16.bin"
#define BC16_CFG_FILE_NAME			"bc16.cfg"
#define BC16_FLASH_TYPE				 STM32G4XX_DUAL_BANK_FLASH_TYPE
#define BC16_PRORGAMM_ADDR			(0x08010000)
#define HAL_USE_FLASH				 1
#define HAL_USE_PAGE_NUMBER_ERASE	 1
#define DACPLAY_DMA_BUF_SIZE		 SOUND_DMA_BUFFER_SIZE
#define ADCPLAY_DMA_BUF_SIZE		 SOUND_DMA_BUFFER_SIZE
#define USE_ADCPLAY_WRITE_DATA		 1


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCU_MUX_CTRL_4_Pin GPIO_PIN_3
#define MCU_MUX_CTRL_4_GPIO_Port GPIOE
#define MCU_MUX_CTRL_5_Pin GPIO_PIN_4
#define MCU_MUX_CTRL_5_GPIO_Port GPIOE
#define Start_Pin GPIO_PIN_6
#define Start_GPIO_Port GPIOE
#define AVAR_Pin GPIO_PIN_13
#define AVAR_GPIO_Port GPIOC
#define MCU_ADC1_1_Pin GPIO_PIN_0
#define MCU_ADC1_1_GPIO_Port GPIOA
#define MCU_ADC2_2_Pin GPIO_PIN_1
#define MCU_ADC2_2_GPIO_Port GPIOA
#define MCU_ADC1_3_Pin GPIO_PIN_2
#define MCU_ADC1_3_GPIO_Port GPIOA
#define P220_LED_Pin GPIO_PIN_3
#define P220_LED_GPIO_Port GPIOA
#define DAC1_2_Pin GPIO_PIN_5
#define DAC1_2_GPIO_Port GPIOA
#define BAT_VOLTAGE_Pin GPIO_PIN_7
#define BAT_VOLTAGE_GPIO_Port GPIOA
#define CHRG_OFF_Pin GPIO_PIN_5
#define CHRG_OFF_GPIO_Port GPIOC
#define MCU_ADC3_1_Pin GPIO_PIN_1
#define MCU_ADC3_1_GPIO_Port GPIOB
#define VIZ_Pin GPIO_PIN_9
#define VIZ_GPIO_Port GPIOE
#define MCU_ADC4_3_Pin GPIO_PIN_12
#define MCU_ADC4_3_GPIO_Port GPIOB
#define CLK_IRQ_Pin GPIO_PIN_15
#define CLK_IRQ_GPIO_Port GPIOB
#define INT1_Pin GPIO_PIN_8
#define INT1_GPIO_Port GPIOD
#define INT2_Pin GPIO_PIN_9
#define INT2_GPIO_Port GPIOD
#define FAULT_Pin GPIO_PIN_0
#define FAULT_GPIO_Port GPIOD
#define FLASH_CS_Pin GPIO_PIN_7
#define FLASH_CS_GPIO_Port GPIOD
#define MCU_MUX_CTRL_3_Pin GPIO_PIN_9
#define MCU_MUX_CTRL_3_GPIO_Port GPIOB
#define MCU_MUX_CTRL_1_Pin GPIO_PIN_0
#define MCU_MUX_CTRL_1_GPIO_Port GPIOE
#define MCU_MUX_CTRL_2_Pin GPIO_PIN_1
#define MCU_MUX_CTRL_2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern ADC_HandleTypeDef hadc4;
extern ADC_HandleTypeDef hadc5;

extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;

extern I2C_HandleTypeDef hi2c3;
extern I2C_HandleTypeDef hi2c4;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

//TODO
//extern IWDG_HandleTypeDef hiwdg;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim15;

extern bool fatfs_mount;

void filelog(char* str);

void USB_CDC_RxCallback(uint8_t* buf, uint32_t len);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
