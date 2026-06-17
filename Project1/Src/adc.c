/*
 * adc.c
 *
 *  Created on: Jun 15, 2026
 *      Author: ryzel
 */
#include "adc.h"
#include "gpio.h"
#include "usart.h"

/*enable bits*/
#define SR_EOC				(1U<<1)

#define CR2_SWSTART			(1U<<30)
#define CR2_ADON			(1U<<0)

#define CR2_CONT 			(1U<<1)

#define ADC_CH0 			(0) //PA0

#define ADC1EN				(1U<<8)



void adc_init(void){
	//clock access to adc module
	RCC_APB2ENR |= ADC1EN;
	//set gpio A0 in gpio.c mode to analog

	//set data resolution
	ADC_CR1 &= ~(1U<<24);
	ADC_CR1 &= ~(1U<<25);

	ADC_CR2 |= CR2_CONT;

	// select the channel 0
	ADC_SQR3 = ADC_CH0;

	//enable adc module
	ADC_CR2 |= CR2_ADON;

}
void adc_conversion(void){
	//set continuous conversion
	ADC_CR2 |= CR2_CONT;

	// enable adc module
	ADC_CR2 |= CR2_SWSTART;
}
int adc_read(void){
	while(!(ADC_SR & SR_EOC))
		;
	return ADC_DR;
}
void adc_sample_signal(char * buff, int * prgm_state){
	char byte_one;
	int uart_byte;

	if (*prgm_state == ON){
		char * ptr = buff;

		for(int x=0; x<64; x++){
			uart_byte = adc_read();
			byte_one = (uart_byte & 0xFF);

			*buff++ = byte_one;
		}
		adc_transmit_signal(ptr);
	}
}
void adc_transmit_signal(char * buff){
	for(int x=0; x<64; x++){
		usart2_write(*buff++);
	}
}




