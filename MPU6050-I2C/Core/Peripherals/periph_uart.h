/*
 * periph_uart.h
 *
 *  Created on: Jun 24, 2026
 *      Author: ryzel
 */

#ifndef PERIPHERALS_PERIPH_UART_H_
#define PERIPHERALS_PERIPH_UART_H_

#include "stm32f4xx_hal.h"

#include <stdbool.h>


#define MAX_BUFFER_LEN 100u
#define MAX_UART_TIMEOUT 100u

bool periph_uart_init(UART_HandleTypeDef *huart);
void periph_uart_handle_tx_int_data(UART_HandleTypeDef *huart);
void periph_uart_handle_rx_int_data(UART_HandleTypeDef *huart);
void periph_uart_send_tx_data(const char *tx_buff, uint16_t buffer_len);

#endif /* PERIPHERALS_PERIPH_UART_H_ */
