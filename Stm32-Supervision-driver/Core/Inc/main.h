/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define  DEBUG(n) do{printf("\r\nDEBUG%d\n",n);}while(0);
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
#define RGB_GPIO_R_Pin GPIO_PIN_13
#define RGB_GPIO_R_GPIO_Port GPIOC
#define RGB_GPIO_G_Pin GPIO_PIN_14
#define RGB_GPIO_G_GPIO_Port GPIOC
#define RGB_GPIO_B_Pin GPIO_PIN_15
#define RGB_GPIO_B_GPIO_Port GPIOC
#define Upload_Button_Pin GPIO_PIN_0
#define Upload_Button_GPIO_Port GPIOC
#define PowerDown_ISR_Pin GPIO_PIN_1
#define PowerDown_ISR_GPIO_Port GPIOC
#define NET_4G_UART_TX_Pin GPIO_PIN_2
#define NET_4G_UART_TX_GPIO_Port GPIOA
#define NET_4G_UART_RX_Pin GPIO_PIN_3
#define NET_4G_UART_RX_GPIO_Port GPIOA
#define BaroMeter_ADC_1_Pin GPIO_PIN_4
#define BaroMeter_ADC_1_GPIO_Port GPIOA
#define BaroMeter_ADC_2_Pin GPIO_PIN_5
#define BaroMeter_ADC_2_GPIO_Port GPIOA
#define BaroMeter_ADC_3_Pin GPIO_PIN_6
#define BaroMeter_ADC_3_GPIO_Port GPIOA
#define BaroMeter_ADC_4_Pin GPIO_PIN_7
#define BaroMeter_ADC_4_GPIO_Port GPIOA
#define GPS_UART_TX_Pin GPIO_PIN_9
#define GPS_UART_TX_GPIO_Port GPIOA
#define GPS_UART_RX_Pin GPIO_PIN_10
#define GPS_UART_RX_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
