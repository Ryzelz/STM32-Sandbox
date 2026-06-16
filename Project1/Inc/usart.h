/*
 * usart.h
 *
 *  Created on: Jun 15, 2026
 *      Author: ryzel
 */

#ifndef USART_H_
#define USART_H_

#include "base.h"
#include <stdint.h>
/* reference manual - usart register */

//control register
#define USART2_CR1_OFFSET 		(0x0C)
#define USART2_CR1 				(*(volatile unsigned int *)(USART2_PERIPHERAL + USART2_CR1_OFFSET))

// baud rate register
#define USART2_BRR_OFFSET 		(0x08)
#define USART2_BRR 				(*(volatile unsigned int *)(USART2_PERIPHERAL + USART2_BRR_OFFSET))

//status register
#define USART2_SR_OFFSET 		(0x00)
#define USART2_SR				(*(volatile unsigned int *)(USART2_PERIPHERAL + USART2_SR_OFFSET))

// data register
#define USART2_DR_OFFSET		(0x04)
#define USART2_DR				(*(volatile unsigned int *)(USART2_PERIPHERAL + USART2_DR_OFFSET))

#define RCC_APB1ENR_OFFSET 		(0x40)
#define RCC_APB1ENR				(*(volatile unsigned int *)(RCC_PERIPHERAL + RCC_APB1ENR_OFFSET))
// functions
void usart2_init(void);
void usart2_write(int ch);
char usart2_read(void);
void usart_set_baudrate(uint32_t periph_clock, uint32_t baudrate);

#endif /* USART_H_ */
