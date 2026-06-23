/*
 * lcd_i2c.c
 *
 *  HD44780 via HLF8574T I2C expander — 4-bit mode
 *  Extra-conservative timing for reliability
 */

#include "lcd_i2c.h"

static I2C_HandleTypeDef *_hi2c;
static uint8_t _backlight = LCD_BIT_BL;

/* ── Raw I2C write ────────────────────────────────────────────────── */
static void LCD_I2C_Write(uint8_t data)
{
    HAL_I2C_Master_Transmit(_hi2c, LCD_I2C_ADDR, &data, 1, 20);
}

/* ── Pulse EN: data must already contain RS/RW/BL bits ───────────── */
static void LCD_Pulse_Enable(uint8_t data)
{
    LCD_I2C_Write(data | LCD_BIT_EN);   /* EN = 1 */
    HAL_Delay(2);
    LCD_I2C_Write(data & ~LCD_BIT_EN);  /* EN = 0 */
    HAL_Delay(2);
}

/* ── Send upper nibble (bits 7-4 map to D7-D4 on expander) ───────── */
static void LCD_Send_Nibble(uint8_t nibble, uint8_t mode)
{
    uint8_t data = (nibble & 0xF0) | mode | _backlight;
    LCD_I2C_Write(data);
    LCD_Pulse_Enable(data);
}

/* ── Send full byte as two nibbles, high nibble first ────────────── */
static void LCD_Send_Byte(uint8_t byte, uint8_t mode)
{
    LCD_Send_Nibble(byte & 0xF0,        mode);
    LCD_Send_Nibble((byte << 4) & 0xF0, mode);
    HAL_Delay(2);
}

static void LCD_Command(uint8_t cmd)  { LCD_Send_Byte(cmd,  0x00); }
static void LCD_Data(uint8_t data)    { LCD_Send_Byte(data, LCD_BIT_RS); }

/* ── Init per HD44780 datasheet, 4-bit mode ──────────────────────── */
void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    _hi2c = hi2c;
    HAL_Delay(100);   /* >40ms after Vcc power-on */

    /*
     * Force 8-bit mode 3 times first (required by HD44780 spec
     * to escape unknown state), then switch to 4-bit.
     * Send as nibbles directly — RS=0, RW=0, BL=1
     */
    LCD_Send_Nibble(0x30, 0x00); HAL_Delay(10);
    LCD_Send_Nibble(0x30, 0x00); HAL_Delay(10);
    LCD_Send_Nibble(0x30, 0x00); HAL_Delay(10);
    LCD_Send_Nibble(0x20, 0x00); HAL_Delay(10);  /* switch to 4-bit */

    LCD_Command(0x28); HAL_Delay(5);   /* 4-bit, 2 lines, 5x8 font */
    LCD_Command(0x08); HAL_Delay(5);   /* display OFF               */
    LCD_Command(0x01); HAL_Delay(5);   /* clear display             */
    LCD_Command(0x06); HAL_Delay(5);   /* entry: increment, no shift*/
    LCD_Command(0x0C); HAL_Delay(5);   /* display ON, cursor OFF    */
}

void LCD_Clear(void)
{
    LCD_Command(0x01);
    HAL_Delay(5);
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
    uint8_t row_offsets[] = { 0x00, 0x40 };
    if (row >= LCD_ROWS) row = LCD_ROWS - 1;
    if (col >= LCD_COLS) col = LCD_COLS - 1;
    LCD_Command(0x80 | (col + row_offsets[row]));
}

void LCD_PrintChar(char c)  { LCD_Data((uint8_t)c); }

void LCD_Print(char *str)
{
    while (*str) LCD_Data((uint8_t)(*str++));
}

void LCD_Backlight(uint8_t state)
{
    _backlight = state ? LCD_BIT_BL : 0x00;
    LCD_I2C_Write(_backlight);
}
