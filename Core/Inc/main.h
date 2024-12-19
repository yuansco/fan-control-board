/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern TIM_HandleTypeDef htim3;
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CH224K_CFG1_Pin GPIO_PIN_0
#define CH224K_CFG1_GPIO_Port GPIOA
#define CH224K_CFG2_Pin GPIO_PIN_1
#define CH224K_CFG2_GPIO_Port GPIOA
#define CH224K_CFG3_Pin GPIO_PIN_2
#define CH224K_CFG3_GPIO_Port GPIOA
#define TEMP_AMB_Pin GPIO_PIN_3
#define TEMP_AMB_GPIO_Port GPIOA
#define TEMP_TARGET_Pin GPIO_PIN_4
#define TEMP_TARGET_GPIO_Port GPIOA
#define VBUS_MON_Pin GPIO_PIN_5
#define VBUS_MON_GPIO_Port GPIOA
#define FAN_TACH_Pin GPIO_PIN_7
#define FAN_TACH_GPIO_Port GPIOA
#define FAN_TACH_EXTI_IRQn EXTI4_15_IRQn
#define FAN_DC_CTRL_Pin GPIO_PIN_0
#define FAN_DC_CTRL_GPIO_Port GPIOB
#define FAN2_DC_CTR_Pin GPIO_PIN_1
#define FAN2_DC_CTR_GPIO_Port GPIOB
#define FAN_PWR_MON_Pin GPIO_PIN_10
#define FAN_PWR_MON_GPIO_Port GPIOB
#define SW_START_STOP_Pin GPIO_PIN_8
#define SW_START_STOP_GPIO_Port GPIOA
#define SW_START_STOP_EXTI_IRQn EXTI4_15_IRQn
#define SW_DOWN_Pin GPIO_PIN_9
#define SW_DOWN_GPIO_Port GPIOA
#define SW_DOWN_EXTI_IRQn EXTI4_15_IRQn
#define SW_UP_Pin GPIO_PIN_10
#define SW_UP_GPIO_Port GPIOA
#define SW_UP_EXTI_IRQn EXTI4_15_IRQn
#define SW_FAN_PWR_Pin GPIO_PIN_12
#define SW_FAN_PWR_GPIO_Port GPIOA
#define SW_FAN_PWR_EXTI_IRQn EXTI4_15_IRQn
#define LED_ERR_Pin GPIO_PIN_15
#define LED_ERR_GPIO_Port GPIOA
#define LED_WORK_Pin GPIO_PIN_0
#define LED_WORK_GPIO_Port GPIOD
#define LED_FAN_PWR_PG_Pin GPIO_PIN_1
#define LED_FAN_PWR_PG_GPIO_Port GPIOD
#define SW_RIGHT_Pin GPIO_PIN_4
#define SW_RIGHT_GPIO_Port GPIOB
#define SW_RIGHT_EXTI_IRQn EXTI4_15_IRQn
#define SW_LEFT_Pin GPIO_PIN_5
#define SW_LEFT_GPIO_Port GPIOB
#define SW_LEFT_EXTI_IRQn EXTI4_15_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
