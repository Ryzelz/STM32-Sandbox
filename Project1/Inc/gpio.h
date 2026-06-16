/*
 * gpio.h
 *
 *  Created on: Jun 15, 2026
 *      Author: ryzel
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <base.h>

/* reference manual GPIO registers*/

/* RCC AHB1 peripheral clock enable in low power mode register (RCC_AHB1LPENR)*/
#define RCC_AHB1ENR_OFFSET 	(0x30)
#define RCC_AHB1ENR 	(*(volatile unsigned int *)(RCC_PERIPHERAL + RCC_AHB1ENR_OFFSET))

/* GPIO port MODE Register */
#define GPIOA_MODER_OFFSET 		(0x0000UL)
#define GPIOA_MODER 			(*(volatile unsigned int *)(GPIOA_PERIPHERAL + GPIOA_MODER_OFFSET))

/* GPIO port Output Data Register */
#define GPIOA_ODR_OFFSET 		(0x14)
#define GPIOA_ODR				(*(volatile unsigned int *)(GPIOA_PERIPHERAL + GPIOA_ODR_OFFSET))

#define GPIOB_MODER_OFFSET 		(0x00)
#define GPIOB_MODER 			(*(volatile unsigned int *)(GPIOB_PERIPHERAL + GPIOB_MODER_OFFSET))

#define GPIOB_ODR_OFFSET 		(0x14)
#define GPIOB_ODR				(*(volatile unsigned int *)(GPIOB_PERIPHERAL + GPIOB_ODR_OFFSET))

#define GPIOC_MODER_OFFSET 		(0x00)
#define GPIOC_MODER 			(*(volatile unsigned int *)(GPIOC_PERIPHERAL + GPIOC_MODER_OFFSET))

/* GPIO port Input Data Register */
#define GPIOC_IDR_OFFSET 		(0x10)
#define GPIOC_IDR				(*(volatile unsigned int *)(GPIOC_PERIPHERAL + GPIOC_IDR_OFFSET))

// Alternative function register
#define GPIOA_AFRL_OFFSET 		(0x20)
#define GPIOA_AFRL				(*(volatile unsigned int *)(GPIOA_PERIPHERAL + GPIOA_AFRL_OFFSET))


void gpio_init(void);
void red_led_on(void);
void red_led_off(void);
void blue_led_on(void);
void blue_led_off(void);
void led_toggle(void);
void prgm_state_set(int * prgm_state);
void prgm_state_dsply(int * prgm_state);
void prgm_state_loop();
void led_delay(void);










#endif /* GPIO_H_ */
