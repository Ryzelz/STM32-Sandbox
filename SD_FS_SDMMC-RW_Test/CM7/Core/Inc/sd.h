/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : SD.h
  * @brief          : Header for SD.c file.
  *                   This file contains the common defines and function
  *                   prototypes for the SD Card (FatFs) driver.
  ******************************************************************************
  * @note
  *   The original monolithic SDIO_SDCard_Test() has been split into the
  *   functions below, following the same file layout used for the I2C
  *   driver (I2C.c / I2C.h):
  *
  *     SD_Mount          - mount the file system
  *     SD_Unmount        - unmount the file system
  *     SD_CardInfo       - report total / free card space
  *     SD_WriteText      - create/overwrite a file with a string
  *     SD_WriteBytes     - create/overwrite a file with raw bytes
  *     SD_ReadText       - read a file back as a string
  *     SD_UpdateText     - append a string to an existing file
  *     SD_ReadBack       - read a file after SD_UpdateText(), to verify it
  *     SD_DeleteFile     - delete a file (implementation left commented out)
  *     SD_RunTest        - demo sequence that calls the above, in place of
  *                         the old SDIO_SDCard_Test()
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_H
#define __SD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include <stdint.h>

/* Exported types --------------------------------------------------------------*/

/* Exported constants -----------------------------------------------------------*/
#define SD_TEST_FILENAME      "test.txt"   /* File used by SD_RunTest()          */
#define SD_TEST_BIN_FILENAME  "data.bin"   /* Binary file used by SD_RunTest()   */
#define SD_READ_BUF_SIZE      256U         /* Default size for text read buffers */

/* Exported macro ----------------------------------------------------------------*/

/* Exported functions prototypes --------------------------------------------------*/

/* Volume level ------------------------------------------------------------------*/
FRESULT SD_Mount(void);
FRESULT SD_Unmount(void);
FRESULT SD_CardInfo(uint32_t *pTotalKB, uint32_t *pFreeKB);

/* File level: text ----------------------------------------------------------------*/
FRESULT SD_WriteText(const char *pFileName, const char *pText);
FRESULT SD_ReadText(const char *pFileName, char *pBuffer, UINT bufferSize);
FRESULT SD_UpdateText(const char *pFileName, const char *pText);
FRESULT SD_ReadBack(const char *pFileName, char *pBuffer, UINT bufferSize);

/* File level: raw bytes -------------------------------------------------------------*/
FRESULT SD_WriteBytes(const char *pFileName, const uint8_t *pData, UINT length);

/* Housekeeping ------------------------------------------------------------------------*/
FRESULT SD_DeleteFile(const char *pFileName);

/* Demo / self-test, replaces the old SDIO_SDCard_Test() --------------------------------*/
void SD_RunTest(void);

#ifdef __cplusplus
}
#endif

#endif /* __SD_H */
