/*
 * gpio.c
 *
 *  Created on: Jun 15, 2026
 *      Author: ryzel
 */


#include <gpio.h>

//Reference manual - RCC AHB1 peripheral clock enable in low power mode register (RCC_AHB1LPENR)
#define GPIOAEN 	(1U<<0)
#define GPIOBEN 	(1U<<1)
#define GPIOCEN		(1U<<2)

//ref man - GPIO port output data register
#define GPIOA8		(1U<<8)
#define GPIOB5		(1U<<5)
//ref man - GPIO port input data register
#define GPIOC13		(1U<<13)
#define GPIOA5 		(1U<<5)

#define RED_LED 	GPIOA8
#define BLUE_LED 	GPIOB5
#define USER_BUTTON GPIOC13
#define USER_LED 	GPIOA5

void gpio_init(void){
	/* Enable clock access to GPIO ports ie A, B C*/

	// enable clock GPIO

	RCC_AHB1ENR |= GPIOAEN;

	RCC_AHB1ENR |= GPIOBEN;

	RCC_AHB1ENR |= GPIOCEN;

	//Set the mode of the Pins
	// refernece manual - GPIO port mode register
	//red led, GPIO8, output
	GPIOA_MODER |= (1U<<16); //1
	GPIOA_MODER &= ~(1U<<17); //0

	//blue led, GPIO5, output
	GPIOB_MODER |= (1U<<10);
	GPIOB_MODER &= ~(1U<<11);

	// input button, GPIO13, input
	GPIOC_MODER &= ~(1U<<26);
	GPIOC_MODER &= ~(1U<<27);

	//set onboard led, port A GPIO5 output
	GPIOA_MODER |= (1U<<10);
	GPIOA_MODER &= ~(1U<<11);

	/* set alternate function register low, AFI */

	// GPIOA2 mode register - alternate function
	GPIOA_MODER &= ~(1U<<4);
	GPIOA_MODER |= (1U<<5);

	// GPIOA3 mode register - alternate function
	GPIOA_MODER &= ~(1U<<6);
	GPIOA_MODER |= (1U<<7);

	// reference manual - GPIO alternate function low register
	//alternate function for PA2, usart_tx
	GPIOA_AFRL &= ~(1U<<11);
	GPIOA_AFRL &= ~(1U<<8);
	GPIOA_AFRL |= (1U<<9);
	GPIOA_AFRL |= (1U<<10);

	//alternative function for PA3, usart_rx
	GPIOA_AFRL &= ~(1U<<15);
	GPIOA_AFRL &= ~(1U<<12);
	GPIOA_AFRL |= (1U<<13);
	GPIOA_AFRL |= (1U<<14);

	// set GPIOA to analog mode, PA0
	GPIOA_MODER |= (3U<<0);

}
void red_led_on(void){
	GPIOA_ODR |= RED_LED;
}
void red_led_off(void){
	GPIOA_ODR &= ~(RED_LED);
}
void blue_led_on(void){
	GPIOB_ODR |= BLUE_LED;
}
void blue_led_off(void){
	GPIOB_ODR &= ~(BLUE_LED);
}
void led_toggle(void){
	GPIOA_ODR ^= USER_LED;

	led_delay();
}
void prgm_state_set(int *prgm_state){
	if(!(GPIOC_IDR & USER_BUTTON)){
		if(*prgm_state == OFF){
			*prgm_state = ON;
			led_delay();
		}	else if(*prgm_state == ON){
			*prgm_state = OFF;
			led_delay();
		}
	}
}
void prgm_state_dsply(int * prgm_state){
	if(*prgm_state == OFF){
		red_led_on();
		blue_led_off();
	}	else if(*prgm_state == ON){
		red_led_off();
		blue_led_on();
	}
}

void prgm_state_loop(){
	for(;;){
		red_led_on();
		blue_led_off();
		led_delay();
		red_led_off();
		blue_led_on();
		led_delay();
	}
}
void led_delay(void){
	for(int x=0; x<150000; x++)
		;
}
