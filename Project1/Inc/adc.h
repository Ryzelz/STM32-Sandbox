/*
 * adc.h
 *
 *  Created on: Jun 15, 2026
 *      Author: ryzel
 */

#ifndef ADC_H_
#define ADC_H_

//refence manual - ADC status register
#define ADC_SR_OFFSET		(0x00)
#define ADC_SR				(*(volatile unsigned int *)(ADC_PERIPHERAL + ADC_SR_OFFSET))

#define ADC_CR1_OFFSET		(0x04)
#define ADC_CR1				(*(volatile unsigned int *)(ADC_PERIPHERAL + ADC_CR1_OFFSET))

#define ADC_CR2_OFFSET		(0x08)
#define ADC_CR2				(*(volatile unsigned int *)(ADC_PERIPHERAL + ADC_CR2_OFFSET))

#define ADC_SQR3_OFFSET 	(0x34)
#define ADC_SQR3			(*(volatile unsigned int *)(ADC_PERIPHERAL + ADC_SQR3_OFFSET))

#define RCC_APB2ENR_OFFSET 	(0x44)
#define RCC_APB2ENR			(*(volatile unsigned int *)(RCC_PERIPHERAL + RCC_APB2ENR_OFFSET))


/**/

void adc_init(void);
void adc_conversion(void);
int adc_read(void);
void adc_sample_signal(char * buff, int * prgm_state);
void adc_transmit_signal(char * buff);






#endif /* ADC_H_ */
