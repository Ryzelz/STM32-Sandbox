/*
 * usart.h
 *
 *  Created on: Jun 15, 2026
 *      Author: ryzel
 */

#ifndef USART_H_
#define USART_H_

#include "base.h"
/* reference manual - usart register */

//control register
#define USART2_CR1_OFFSET 		(0x0C)
#define USART2_CR1 				(*(volatile unsigned int *)(USART2_PERIPHERAL + USART2_CR1_OFFSET))

// baud rate register
#define USART2_BRR_OFFSET 		(0x08)
#define USART2_BRR 				(*(volatile unsigned int *)(USART2_PERIPHERAL + USART2_BRR_OFFSET))


#endif /* USART_H_ */
