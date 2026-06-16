/*
 * usart.c
 *
 *  Created on: Jun 15, 2026
 *      Author: ryzel
 */

//Enable bits according to the chart
// reference manual - USART Control register

#include "usart.h"

#define CR1_UE 		(1U<<13)
#define CR1_RE		(1U<<2)
#define CR1_TE		(1U<<3)

#define SR_RXNE 	(1U<<5)
#define SR_TXE 		(1U<<7)

#define RCC_USART2_EN 	(1U<<2)

#define SYS_FREQ 			16000000
#define PERIPHERAL_CLOCK 	SYS_FREQ
#define BAUDRATE			115200

// functions
void usart2_init(void){
	//allow clock access to usart peripheral
	RCC_APB1ENR |= RCC_USART2_EN;


	//set usart2 alternate function in gpio.c
	// datasheet manual - alternate function mapping
	// AF7, usart2_tx = PA2, usart2_rx = PA3
	// reference manual - gpio alternate func low register


	//config baudrate
	usart_set_baudrate(PERIPHERAL_CLOCK, BAUDRATE);

	// enable tx and rx transfer function in cr1
	USART2_CR1 |= CR1_TE;
	USART2_CR1 |= CR1_RE;

	//enable usart2 module
	USART2_CR1 |= CR1_UE;
}
void usart2_write(int ch){
	// make sure the transmit data register is not empty
	while(!(USART2_SR & SR_TXE))
		;

	USART2_DR = (ch & 0xFF);

}
char usart2_read(void){
	while(!(USART2_SR & SR_RXNE))
		;

	return USART2_DR;
}
void usart_set_baudrate(uint32_t periph_clock, uint32_t baudrate){
	USART2_BRR = ((periph_clock + (baudrate/2U)) / baudrate);

}
