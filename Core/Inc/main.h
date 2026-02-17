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
#define MCU_MUX_CTRL_3_Pin GPIO_PIN_2
#define MCU_MUX_CTRL_3_GPIO_Port GPIOE
#define MCU_MUX_CTRL_4_Pin GPIO_PIN_3
#define MCU_MUX_CTRL_4_GPIO_Port GPIOE
#define MCU_MUX_CTRL_5_Pin GPIO_PIN_4
#define MCU_MUX_CTRL_5_GPIO_Port GPIOE
#define Avar_Pin GPIO_PIN_5
#define Avar_GPIO_Port GPIOE
#define Start_Pin GPIO_PIN_6
#define Start_GPIO_Port GPIOE
#define MCU_ADC1_1_Pin GPIO_PIN_0
#define MCU_ADC1_1_GPIO_Port GPIOA
#define MCU_ADC2_2_Pin GPIO_PIN_1
#define MCU_ADC2_2_GPIO_Port GPIOA
#define MCU_ADC1_3_Pin GPIO_PIN_2
#define MCU_ADC1_3_GPIO_Port GPIOA
#define P220_LED_Pin GPIO_PIN_3
#define P220_LED_GPIO_Port GPIOA
#define DAC1_1_Pin GPIO_PIN_4
#define DAC1_1_GPIO_Port GPIOA
#define DAC1_2_Pin GPIO_PIN_5
#define DAC1_2_GPIO_Port GPIOA
#define BAT_VOLTAGE_Pin GPIO_PIN_7
#define BAT_VOLTAGE_GPIO_Port GPIOA
#define BAT_CHARGED_Pin GPIO_PIN_4
#define BAT_CHARGED_GPIO_Port GPIOC
#define MPPT_OFF_Pin GPIO_PIN_5
#define MPPT_OFF_GPIO_Port GPIOC
#define MCU_ADC3_1_Pin GPIO_PIN_1
#define MCU_ADC3_1_GPIO_Port GPIOB
#define VIZ_Pin GPIO_PIN_10
#define VIZ_GPIO_Port GPIOE
#define MCU_ADC4_3_Pin GPIO_PIN_12
#define MCU_ADC4_3_GPIO_Port GPIOB
#define mute_OP_Pin GPIO_PIN_15
#define mute_OP_GPIO_Port GPIOB
#define INT1_Pin GPIO_PIN_8
#define INT1_GPIO_Port GPIOD
#define INT2_Pin GPIO_PIN_9
#define INT2_GPIO_Port GPIOD
#define MCU_ADC5_1_Pin GPIO_PIN_8
#define MCU_ADC5_1_GPIO_Port GPIOA
#define CLK_IRQ_Pin GPIO_PIN_11
#define CLK_IRQ_GPIO_Port GPIOC
#define FREE_GPIO_1_Pin GPIO_PIN_0
#define FREE_GPIO_1_GPIO_Port GPIOD
#define FREE_GPIO_2_Pin GPIO_PIN_1
#define FREE_GPIO_2_GPIO_Port GPIOD
#define FREE_GPIO_3_Pin GPIO_PIN_2
#define FREE_GPIO_3_GPIO_Port GPIOD
#define SPI_CS_Pin GPIO_PIN_7
#define SPI_CS_GPIO_Port GPIOD
#define MCU_MUX_CTRL_1_Pin GPIO_PIN_0
#define MCU_MUX_CTRL_1_GPIO_Port GPIOE
#define MCU_MUX_CTRL_2_Pin GPIO_PIN_1
#define MCU_MUX_CTRL_2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
