/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : DHT11 -> I2C LCD 16x2 (HLF8574T backpack)
  *
  *  Pinout:
  *    DHT11 Data  -> PA6  (10kΩ pull-up to 3.3V)
  *    LCD SDA     -> PB7  (I2C1)
  *    LCD SCL     -> PB6  (I2C1)
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"

/* USER CODE BEGIN Includes */
#include "dht11.h"
#include "lcd_i2c.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

I2C_HandleTypeDef  hi2c1;
TIM_HandleTypeDef  htim1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Raw values stored as tenths: 450 = 45.0%, 280 = 28.0C */
int16_t hum_raw  = 0;
int16_t temp_raw = 0;
char    lcd_buf[17];
char    uart_buf[64];
/* USER CODE END PV */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);

/* USER CODE BEGIN 0 */

/* ── LCD display: DHT11 reading ──────────────────────────────────── */
/* Row 0: "Hum:  45.0 %    "                                          */
/* Row 1: "T: 28.0C  82.4F "                                          */
void LCD_ShowDHT11(int16_t hum, int16_t temp)
{
    int hum_i  = hum  / 10;
    int hum_d  = hum  % 10;
    int temp_i = temp / 10;
    int temp_d = temp % 10;

    /* Fahrenheit: (C * 9/5) + 32, keeping tenths precision */
    int32_t tempF_raw = (int32_t)temp * 9 / 5 + 320;
    int tempF_i = (int)(tempF_raw / 10);
    int tempF_d = (int)(tempF_raw % 10);

    LCD_SetCursor(0, 0);
    snprintf(lcd_buf, sizeof(lcd_buf), "Hum:  %2d.%d %%    ", hum_i, hum_d);
    LCD_Print(lcd_buf);

    LCD_SetCursor(0, 1);
    snprintf(lcd_buf, sizeof(lcd_buf), "T:%2d.%dC  %3d.%dF", temp_i, temp_d, tempF_i, tempF_d);
    LCD_Print(lcd_buf);
}

/* ── LCD display: sensor error ───────────────────────────────────── */
void LCD_ShowError(void)
{
    LCD_SetCursor(0, 0);
    LCD_Print("  Sensor Error! ");
    LCD_SetCursor(0, 1);
    LCD_Print(" Check wiring.. ");
}

/* USER CODE END 0 */

/* ── Entry point ──────────────────────────────────────────────────── */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();

    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start(&htim1);
    HAL_Delay(200);   /* let peripherals settle */

    /* LCD splash screen */
    LCD_Init(&hi2c1);
    LCD_SetCursor(0, 0);
    LCD_Print("  DHT11 + LCD   ");
    LCD_SetCursor(0, 1);
    LCD_Print("  Starting...   ");
    HAL_Delay(2000);
    LCD_Clear();
    /* USER CODE END 2 */

    while (1)
    {
        /* Blink onboard LED to show loop is alive */
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

        if (DHT11_GetData(&hum_raw, &temp_raw))
        {
            /* ── Valid reading ── */
            int hum_i  = hum_raw  / 10;
            int hum_d  = hum_raw  % 10;
            int temp_i = temp_raw / 10;
            int temp_d = temp_raw % 10;

            int32_t tempF_raw = (int32_t)temp_raw * 9 / 5 + 320;
            int tempF_i = (int)(tempF_raw / 10);
            int tempF_d = (int)(tempF_raw % 10);

            /* Output to UART serial monitor */
            snprintf(uart_buf, sizeof(uart_buf),
                     "Hum: %d.%d%%  Temp: %d.%dC / %d.%dF\r\n",
                     hum_i, hum_d, temp_i, temp_d, tempF_i, tempF_d);
            HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, strlen(uart_buf), 100);

            /* Output to LCD */
            LCD_ShowDHT11(hum_raw, temp_raw);
        }
        else
        {
            /* ── Sensor error ── */
            const char *err = "DHT11 error! Check wiring.\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)err, strlen(err), 100);
            LCD_ShowError();
        }

        /* DHT11 minimum sampling interval = 1 second */
        HAL_Delay(1000);
    }
}

/* ── Peripheral init ──────────────────────────────────────────────── */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM      = 4;
    RCC_OscInitStruct.PLL.PLLN      = 84;
    RCC_OscInitStruct.PLL.PLLP      = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ      = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

static void MX_I2C1_Init(void)
{
    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = 100000;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2     = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) Error_Handler();
}

static void MX_TIM1_Init(void)
{
    TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig      = {0};

    htim1.Instance               = TIM1;
    htim1.Init.Prescaler         = 83;    /* 84MHz/(83+1) = 1MHz = 1us/tick */
    htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim1.Init.Period            = 65535;
    htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK) Error_Handler();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 115200;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) Error_Handler();
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Onboard LED LD2 */
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin   = LD2_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

    /* User button B1 */
    GPIO_InitStruct.Pin  = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    /* PA6 - DHT11 data (direction set dynamically in dht11.c) */
    GPIO_InitStruct.Pin  = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}
