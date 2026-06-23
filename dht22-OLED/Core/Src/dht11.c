/*
 * dht11.c
 *
 *  Created on: Jun 23, 2026
 *      Author: ryzel
 */


/*
 *
 *  DHT11 sensor driver
 *  Data Pin: PA6 (GPIOA, GPIO_PIN_6)
 *
 *  DHT11 packet: [RH_H][RH_L][T_H][T_L][CHK]
 *  Values are whole integers (no tenths like DHT22)
 *  e.g. RH_H = 45 means 45%, T_H = 28 means 28C
 */

#include "dht11.h"

/* htim1 is defined in main.c — declared extern here for microDelay */
extern TIM_HandleTypeDef htim1;

/* ── Microsecond delay via TIM1 (84MHz / (83+1) = 1MHz = 1us/tick) ── */
static void microDelay(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

/* ── Set PA6 as push-pull output ─────────────────────────────────── */
void DHT11_SetOutput(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin   = DHT11_PIN;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    g.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_PORT, &g);
}

/* ── Set PA6 as input with pull-up ───────────────────────────────── */
void DHT11_SetInput(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin  = DHT11_PIN;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PORT, &g);
}

/* ── DHT11 start sequence + ACK check ────────────────────────────── */
/* Host pulls LOW for 18ms, releases, sensor responds LOW then HIGH   */
/* Returns 1 if sensor ACK'd, 0 if no response                        */
uint8_t DHT11_Start(void)
{
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(18);                  /* DHT11 needs >18ms LOW         */
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    microDelay(30);                 /* release and wait 20-40us      */

    DHT11_SetInput();
    microDelay(40);

    if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) return 0; /* no ACK */

    microDelay(80);                 /* wait through 80us LOW         */
    if (!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) return 0;

    microDelay(80);                 /* wait through 80us HIGH        */
    return 1;
}

/* ── Read one byte from DHT11, MSB first ─────────────────────────── */
uint8_t DHT11_Read(void)
{
    uint8_t value = 0;
    for (int i = 0; i < 8; i++)
    {
        /* Wait for bit-start: LOW -> HIGH transition (~50us LOW) */
        uint32_t t = HAL_GetTick();
        while (!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))
            if (HAL_GetTick() - t > 3) break;   /* timeout guard */

        /* Sample at 35us: <28us = '0', >28us = '1' */
        microDelay(35);
        if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))
            value |= (1 << (7 - i));

        /* Wait for HIGH to end before next bit */
        t = HAL_GetTick();
        while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))
            if (HAL_GetTick() - t > 3) break;
    }
    return value;
}

/* ── Read full 5-byte packet and validate checksum ───────────────── */
/* DHT11: [RH_int][RH_dec][T_int][T_dec][CHK]                         */
/* RH_dec and T_dec are always 0 for DHT11 (no decimal precision)     */
/* Returns 1 on success, 0 on checksum fail or no response            */
uint8_t DHT11_GetData(int16_t *hum, int16_t *temp)
{
    if (!DHT11_Start()) return 0;

    uint8_t rh_h = DHT11_Read();   /* humidity integer     */
    uint8_t rh_l = DHT11_Read();   /* humidity decimal (0) */
    uint8_t t_h  = DHT11_Read();   /* temperature integer  */
    uint8_t t_l  = DHT11_Read();   /* temperature decimal (0) */
    uint8_t chk  = DHT11_Read();   /* checksum             */

    /* Checksum = lower 8 bits of sum of first 4 bytes */
    if (((rh_h + rh_l + t_h + t_l) & 0xFF) != chk) return 0;

    /* DHT11 gives whole numbers only — store as tenths for LCD format */
    *hum  = (int16_t)(rh_h * 10);   /* e.g. 45% -> 450 */
    *temp = (int16_t)(t_h  * 10);   /* e.g. 28C -> 280 */

    return 1;
}
