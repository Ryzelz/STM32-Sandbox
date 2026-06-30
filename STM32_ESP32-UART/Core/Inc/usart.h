/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */
#define STATUS_LED_GPIO_Port GPIOA
#define STATUS_LED_Pin       GPIO_PIN_5

#define LED_TIMEOUT_MS 200

extern char Rx_byte;
extern char Tx_confirmation_buffer[32];
extern uint32_t last_rx_time;
extern char tx_data[];

/* USER CODE END Private defines */

void MX_USART6_UART_Init(void);

/* USER CODE BEGIN Prototypes */

void MX_GPIO_Init(void);
void MPU_Config(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

