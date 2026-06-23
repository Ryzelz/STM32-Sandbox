/*
 * dht11.h
 *
 *  Created on: Jun 23, 2026
 *      Author: ryzel
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "main.h"
#include <stdint.h>

#define DHT11_PORT  GPIOA
#define DHT11_PIN   GPIO_PIN_6

void    DHT11_SetOutput(void);
void    DHT11_SetInput(void);
uint8_t DHT11_Start(void);
uint8_t DHT11_Read(void);
uint8_t DHT11_GetData(int16_t *hum, int16_t *temp);



#endif /* INC_DHT11_H_ */
