/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "sdmmc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "RingBuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Sampling frequency of ADC/period of timer */
#define SAMP_FREQ 			    40000

/* DMA buffer length. Since app uses double buffer, amount of time to transmit ADC data to SD card is
 * ( 1 / SAMP_FREQ ) * ( DMA_BUFF_LENGTH / 2)
 * Make buffer length a power of 2 */
#define DMA_BUFF_LENGTH		  8192

/*
 * The size of the ring buffer in terms of DMA buffer lengths
 * ie. if RING_BUFFER_CHUNKS = 2, total length of ring buffer is DDMA_BUFF_LENGTH * RING_BUFFER_CHUNKS
 */
#define RING_BUFFER_CHUNKS  8


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Lookup table for decimal to string conversion
 * doing two digits at a time reduces number of divides, improving performance
 */
static const char digits_LUT[201] =

  "0001020304050607080910111213141516171819"

  "2021222324252627282930313233343536373839"

  "4041424344454647484950515253545556575859"

  "6061626364656667686970717273747576777879"

  "8081828384858687888990919293949596979899";

Ring_Buffer_t hRingBuff =
  {
      .BytesPerSample =   2,                    // 12 bit ADC data will use 16-bit data buffer
      .MaxNumSamples =    DMA_BUFF_LENGTH * RING_BUFFER_CHUNKS,  // make larger than DMA buffer to help avoid data loss

      /* clear all index variables */
      .readIdx =          0,
      .writeIdx =         0,
      .availableSamples = 0
  };

uint32_t RingBufferErrorCode;



FRESULT res; /* FatFs function common result code */
UINT byteswritten; /* File write/read counts */
const char header[] = "adc_value\n";  /* CSV Column Labels */
char str_buf[16];
uint8_t workBuffer[_MAX_SS];
FIL ADC_data_file;

volatile uint16_t adc_dma_buf[DMA_BUFF_LENGTH];
uint16_t *ConsumeBuff;

uint8_t write_adc_data = 0;
int file_num = 0;
uint32_t str_len;

int ADCStrlength;
char ADCValStr[16];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/*
 * Control the sampling frequncy of ADC by adjusting timer parameters.
 */
void SetSamplingFrequency(uint32_t freq);

/*
 * Initialize File System and Link low level SD drivers
 */
FRESULT InitFileSystem(void);

// Custom fast itoa function
int fast4DigitDecToStr(uint32_t value, char* dst);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * Default generated SD detection does not work. Redefined weakly defined function here
 */
uint8_t BSP_SD_IsDetected(void)
{
  __IO uint8_t status = SD_PRESENT;

  if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
  {
	  status = SD_NOT_PRESENT;
  }

  return status;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  int32_t timeout;
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_0 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDMMC2_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  // Program will not start until there is an SD card present
  while (!BSP_SD_IsDetected()) {}

  // Link SD drivers to file system
  res = InitFileSystem();

  if (res != FR_OK)
  {
    // Error initializing file system
    Error_Handler();
  }

  // Set the sampling frequency of the ADC by setting timer frequency
  SetSamplingFrequency(SAMP_FREQ);

  // Allocate the memory for the ring buffer
  RingBufferErrorCode = RingBuffer_alloc(&hRingBuff);

  if ( RingBufferErrorCode != RB_ERR_OK )
  {
    Error_Handler();
  }

  // Start timer that triggers ADC
  if (HAL_TIM_Base_Start(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  // Calibrate ADC
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }

  // Start the ADC DMA transfer
  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buf, DMA_BUFF_LENGTH) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    // Start writing ADC data when button is pressed, flag toggled in EXTI ISR
    if (write_adc_data)
    {
      file_num++;

      // green LED indicates that file writing process is ongoing
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

      str_len = sprintf(str_buf, "ADCVals%d.csv", file_num);

      char file_name[str_len+1];

      // Necessary to make sure string is NULL terminated
      for (uint32_t i = 0; i < str_len + 1; i++)
      {
          file_name[i] = str_buf[i];
      }

      res = f_open(&ADC_data_file, file_name, FA_CREATE_ALWAYS | FA_WRITE);

      if( res == FR_OK)
      {
        res = f_write(&ADC_data_file, header, sizeof(header), &byteswritten);
        if (res != FR_OK)
        {
          Error_Handler();
        }


        /* Write ADC data into file until the button is pressed again */
        while ( write_adc_data )
        {

          // Avoid underflow of ring buffer
          if ( hRingBuff.availableSamples > 0 )
          {
            ConsumeBuff = (uint16_t *)RingBuffer_consume(&hRingBuff, DMA_BUFF_LENGTH/2);

            /* Extra safety net.
             * Should be hit since above conditionals checks or available samples
             */
            if (ConsumeBuff == NULL)
            {
              Error_Handler();
            }

            for (uint32_t i = 0; i < DMA_BUFF_LENGTH/2; i++)
            {
              /*
               * Custom itoa implementation, only works for base 10 decimal less than 5 digits.
               */
              ADCStrlength = fast4DigitDecToStr((int)ConsumeBuff[i], ADCValStr);

              ADCValStr[ADCStrlength] = '\n';

              // Write longer than string end to include null terminator
              res = f_write(&ADC_data_file, ADCValStr, (ADCStrlength + 1), &byteswritten);

            }
          }

        }

        /*
         * If the above while loop finishes executing and there are still samples left in the ring buffer,
         * finish writing the remaining samples to the file
         */
        if ( hRingBuff.availableSamples > 0 )
        {
          while (hRingBuff.availableSamples > 0)
          {

            ConsumeBuff = (uint16_t *)RingBuffer_consume(&hRingBuff, DMA_BUFF_LENGTH/2);

            /* Extra safety net.
             * Should be hit since above conditionals checkf or available samples
             */
            if (ConsumeBuff == NULL)
            {
              Error_Handler();
            }

            for (uint32_t i = 0; i < DMA_BUFF_LENGTH/2; i++)
            {

              ADCStrlength = fast4DigitDecToStr((int)ConsumeBuff[i], ADCValStr);

              ADCValStr[ADCStrlength] = '\n';

              res = f_write(&ADC_data_file, ADCValStr, (ADCStrlength + 1), &byteswritten);

            }

          }
        }

        f_close(&ADC_data_file);
      }

      // Turn off green LED to indicate file isn't being written to any longer
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 4096;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void SetSamplingFrequency(uint32_t freq)
{
	uint32_t prescaler = 0;

	// 120MHz clock frequency
	uint32_t ARRval = (120000000 / ( (freq) * (prescaler + 1) ) ) - 1;

	/*
	 * If ARR value associated with proper frequency is larger than max allowed
	 * ARR value, then increase the prescaler until ARR value is within acceptable range
	 */
	while ( ARRval >= 65535 )
	{
		prescaler++;
		__HAL_TIM_SET_PRESCALER(&htim3, prescaler);
		ARRval = (120000000 / ( (freq) * (prescaler + 1) ) ) - 1;
	}

	__HAL_TIM_SET_AUTORELOAD(&htim3, ARRval);

}

FRESULT InitFileSystem(void)
{
  FRESULT res;
  res = f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));

  if (res != FR_OK)
  {
    return res;
  }

  res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);

  if(res != FR_OK)
  {
    return res;
  }

  return res;
}

/*
 * Fast implementation of an itoa type of function with LUT. Returns the length of number turned to string
 * NOTE this only works for base 10 numbers 9999 or less
 */
int fast4DigitDecToStr(uint32_t value, char* dst) {


  uint32_t length;

  /* Comparison is much faster than mathematical calculation of length of number
   * Since number is only 4 digits at most, no needs to check further
   */
  if (value < 10) length = 1;

  else if (value < 100) length = 2;

  else if (value < 1000) length = 3;

  else if (value < 10000) length = 4;

  uint32_t str_idx = length - 1;

  /* Work buffer backwards for better performance, and use lookup table of two
   * digits at a time to reduce number of divides
   */
  while (value >= 100) {

    uint32_t digit_idx = (value % 100) * 2;

    value /= 100;   // Divide by 100 since we are handling 2 digits at a time

    dst[str_idx] = digits_LUT[digit_idx + 1];

    dst[str_idx - 1] = digits_LUT[digit_idx];

    str_idx -= 2;

  }

  // Handle last 1-2 digits

  // Only one digit left to handle
  if (value < 10) {

    dst[str_idx] = '0' + value;

  }
  // Handle last 2 digits with LUT
  else {

    uint32_t digit_idx = value * 2;

    dst[str_idx] = digits_LUT[digit_idx + 1];

    dst[str_idx - 1] = digits_LUT[digit_idx];

  }

  return (int)length;

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  //Toggles ADC write state when user button is pressed
  write_adc_data ^= 1;
}

static void Handle_Overflow(void)
{
  // Stop writing ADC since internal ring buffer can't handle more data
   write_adc_data = 0;

  /* Reset all of the ring buffer state variables
   * in prep for restarting ADC reading
   */
  hRingBuff.readIdx = 0;
  hRingBuff.writeIdx = 0;
  hRingBuff.availableSamples = 0;

  // Blink a few times to indicate overflow.
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (write_adc_data)
  {
    RingBufferErrorCode = RingBuffer_feed(&hRingBuff, (uint8_t *)&adc_dma_buf[DMA_BUFF_LENGTH/2], DMA_BUFF_LENGTH/2);
    if( RingBufferErrorCode != RB_ERR_OK )
    {
      if (RingBufferErrorCode == RB_ERR_OVERFLOW)
      {
        Handle_Overflow();
      }
    }
  }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (write_adc_data)
  {
    RingBufferErrorCode = RingBuffer_feed(&hRingBuff, (uint8_t *)&adc_dma_buf[0], DMA_BUFF_LENGTH/2);
    if( RingBufferErrorCode != RB_ERR_OK )
    {
      if (RingBufferErrorCode == RB_ERR_OVERFLOW)
      {
        Handle_Overflow();
      }
    }
  }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  Error_Handler();
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
