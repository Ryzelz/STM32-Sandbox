/*
 * lcd_i2c.h
 *
 *  LCD 16x2 HD44780 driver via HLF8574T I2C expander
 *  I2C Address: 0x27
 *
 *  Wiring:
 *    LCD Module  ->  STM32
 *    VCC         ->  5V
 *    GND         ->  GND
 *    SDA         ->  PB7 (I2C1 SDA)
 *    SCL         ->  PB6 (I2C1 SCL)
 */

#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include "main.h"
#include <stdint.h>

/* HLF8574T I2C address (7-bit shifted left for HAL) */
#define LCD_I2C_ADDR        (0x27 << 1)

/* LCD dimensions */
#define LCD_COLS            16
#define LCD_ROWS            2

/*
 * HLF8574T pin mapping to HD44780
 * P0 -> RS
 * P1 -> RW
 * P2 -> EN
 * P3 -> Backlight
 * P4 -> D4
 * P5 -> D5
 * P6 -> D6
 * P7 -> D7
 */
#define LCD_BIT_RS          (1 << 0)
#define LCD_BIT_RW          (1 << 1)
#define LCD_BIT_EN          (1 << 2)
#define LCD_BIT_BL          (1 << 3)
#define LCD_BIT_D4          (1 << 4)
#define LCD_BIT_D5          (1 << 5)
#define LCD_BIT_D6          (1 << 6)
#define LCD_BIT_D7          (1 << 7)

/* LCD commands */
#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_ENTRY_MODE      0x06
#define LCD_DISPLAY_ON      0x0C
#define LCD_FUNCTION_SET    0x28
#define LCD_SET_DDRAM       0x80

/* Row DDRAM offsets */
#define LCD_ROW0_OFFSET     0x00
#define LCD_ROW1_OFFSET     0x40

void LCD_Init(I2C_HandleTypeDef *hi2c);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(char *str);
void LCD_PrintChar(char c);
void LCD_Backlight(uint8_t state);

#endif /* LCD_I2C_H_ */
