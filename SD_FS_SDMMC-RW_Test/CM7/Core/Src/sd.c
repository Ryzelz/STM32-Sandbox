/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : SD.c
  * @brief          : SD Card (FatFs) driver
  ******************************************************************************
  * @note
  *   If your project already generates fatfs.c via CubeMX
  *   (MX_FATFS_Init() / FATFS_LinkDriver()), it already defines SDFatFs and
  *   SDPath. In that case, delete the two "Private variables" below and
  *   replace them with:
  *       extern FATFS SDFatFs;
  *       extern char  SDPath[4];
  *   to avoid a duplicate-definition error at link time.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "sd.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -------------------------------------------------------------*/
/* Private define ---------------------------------------------------------------*/
/* Private macro ------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------*/
static FATFS SDFatFs;   /* File system object for the SD logical drive */
static char  SDPath[4]; /* SD logical drive path, e.g. "0:/"            */

/* Private function prototypes -------------------------------------------------------*/

/* Exported functions ----------------------------------------------------------------*/

/* USER CODE BEGIN Volume level */

/**
  * @brief  Mounts the SD card file system.
  * @note   Uses immediate mounting (opt = 1) so a mount error is reported
  *         right away instead of being deferred to the first file access.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_Mount(void)
{
  FRESULT res;

  res = f_mount(&SDFatFs, SDPath, 1);

  return res;
}

/**
  * @brief  Unmounts the SD card file system.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_Unmount(void)
{
  FRESULT res;

  res = f_mount(NULL, SDPath, 0);

  return res;
}

/**
  * @brief  Reads total and free space of the mounted SD card.
  * @note   Assumes 512-byte sectors (standard for SD cards), hence the
  *         "* 0.5f" conversion from sectors to KB.
  * @param  pTotalKB Pointer to store total card space, in KB.
  * @param  pFreeKB  Pointer to store free card space, in KB.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_CardInfo(uint32_t *pTotalKB, uint32_t *pFreeKB)
{
  FRESULT res;
  FATFS *pFs;
  DWORD freeClusters;

  res = f_getfree(SDPath, &freeClusters, &pFs);

  if (res == FR_OK)
  {
    *pTotalKB = (uint32_t)((pFs->n_fatent - 2) * pFs->csize * 0.5f);
    *pFreeKB  = (uint32_t)(freeClusters * pFs->csize * 0.5f);
  }

  return res;
}

/* USER CODE END Volume level */

/* USER CODE BEGIN File level text */

/**
  * @brief  Creates (or overwrites) a text file and writes a string into it.
  * @param  pFileName Name of the file to write, e.g. "test.txt".
  * @param  pText     Null-terminated string to write.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_WriteText(const char *pFileName, const char *pText)
{
  FRESULT res;
  FIL file;

  res = f_open(&file, pFileName, FA_CREATE_ALWAYS | FA_WRITE);
  if (res != FR_OK)
  {
    return res;
  }

  if (f_puts(pText, &file) < 0)
  {
    res = FR_INT_ERR;
  }

  f_close(&file);

  return res;
}

/**
  * @brief  Opens an existing text file and reads its content as a string.
  * @param  pFileName  Name of the file to read.
  * @param  pBuffer    Destination buffer (null-terminated by f_gets).
  * @param  bufferSize Size of pBuffer, in bytes.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_ReadText(const char *pFileName, char *pBuffer, UINT bufferSize)
{
  FRESULT res;
  FIL file;

  res = f_open(&file, pFileName, FA_READ);
  if (res != FR_OK)
  {
    return res;
  }

  if (f_gets(pBuffer, (int)bufferSize, &file) == NULL)
  {
    res = FR_INT_ERR;
  }

  f_close(&file);

  return res;
}

/**
  * @brief  Opens an existing text file (creating it if needed) and appends
  *         a string to the end of it, without touching existing content.
  * @param  pFileName Name of the file to update.
  * @param  pText     Null-terminated string to append.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_UpdateText(const char *pFileName, const char *pText)
{
  FRESULT res;
  FIL file;

  res = f_open(&file, pFileName, FA_OPEN_ALWAYS | FA_WRITE);
  if (res != FR_OK)
  {
    return res;
  }

  /* Move the pointer to the end of the file before writing */
  res = f_lseek(&file, f_size(&file));
  if (res != FR_OK)
  {
    f_close(&file);
    return res;
  }

  if (f_puts(pText, &file) < 0)
  {
    res = FR_INT_ERR;
  }

  f_close(&file);

  return res;
}

/**
  * @brief  Reads a file back after SD_UpdateText(), so the update can be
  *         verified. Functionally the same as SD_ReadText(), kept as its
  *         own call so the test/demo sequence reads clearly.
  * @param  pFileName  Name of the file to read.
  * @param  pBuffer    Destination buffer (null-terminated by f_gets).
  * @param  bufferSize Size of pBuffer, in bytes.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_ReadBack(const char *pFileName, char *pBuffer, UINT bufferSize)
{
  return SD_ReadText(pFileName, pBuffer, bufferSize);
}

/* USER CODE END File level text */

/* USER CODE BEGIN File level bytes */

/**
  * @brief  Creates (or overwrites) a file and writes raw bytes into it.
  * @param  pFileName Name of the file to write.
  * @param  pData     Pointer to the byte buffer to write.
  * @param  length    Number of bytes to write.
  * @retval FRESULT FatFs operation result, FR_OK on success.
  */
FRESULT SD_WriteBytes(const char *pFileName, const uint8_t *pData, UINT length)
{
  FRESULT res;
  FIL file;
  UINT bytesWritten;

  res = f_open(&file, pFileName, FA_CREATE_ALWAYS | FA_WRITE);
  if (res != FR_OK)
  {
    return res;
  }

  res = f_write(&file, pData, length, &bytesWritten);
  if ((res == FR_OK) && (bytesWritten != length))
  {
    res = FR_DISK_ERR; /* Not all requested bytes were written */
  }

  f_close(&file);

  return res;
}

/* USER CODE END File level bytes */

/* USER CODE BEGIN Housekeeping */

/**
  * @brief  Deletes a file from the SD card.
  * @note   Disabled by default for safety, as requested: the f_unlink()
  *         call is left commented out below. Uncomment it to enable real
  *         deletion.
  * @param  pFileName Name of the file to delete.
  * @retval FRESULT Always FR_OK while disabled; the real FatFs result
  *         once the f_unlink() line is uncommented.
  */
FRESULT SD_DeleteFile(const char *pFileName)
{
  FRESULT res = FR_OK;

  /* res = f_unlink(pFileName); */

  return res;
}

/* USER CODE END Housekeeping */

/* USER CODE BEGIN SD_RunTest */

/**
  * @brief  Demo / self-test sequence, equivalent to the original monolithic
  *         SDIO_SDCard_Test(), now built from the modular SD_xxx functions
  *         above. Call once, e.g. from main(), to confirm the SD card and
  *         file system are working.
  * @note   Assumes printf() has been retargeted (e.g. to a UART) as is
  *         common in CubeMX projects; remove the printf() calls if not.
  * @retval None
  */
void SD_RunTest(void)
{
  FRESULT  res;
  uint32_t totalKB, freeKB;
  char     readBuf[SD_READ_BUF_SIZE];
  uint8_t  byteBuf[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

  res = SD_Mount();
  if (res != FR_OK)
  {
    printf("SD_Mount failed: %d\r\n", res);
    return;
  }

  res = SD_CardInfo(&totalKB, &freeKB);
  if (res == FR_OK)
  {
    printf("SD card: %lu KB total, %lu KB free\r\n",
           (unsigned long)totalKB, (unsigned long)freeKB);
  }

  res = SD_WriteText(SD_TEST_FILENAME, "Hello from SD_WriteText\r\n");
  if (res != FR_OK)
  {
    printf("SD_WriteText failed: %d\r\n", res);
  }

  res = SD_WriteBytes(SD_TEST_BIN_FILENAME, byteBuf, sizeof(byteBuf));
  if (res != FR_OK)
  {
    printf("SD_WriteBytes failed: %d\r\n", res);
  }

  res = SD_ReadText(SD_TEST_FILENAME, readBuf, sizeof(readBuf));
  if (res == FR_OK)
  {
    printf("Read back: %s\r\n", readBuf);
  }

  res = SD_UpdateText(SD_TEST_FILENAME, "Appended line\r\n");
  if (res != FR_OK)
  {
    printf("SD_UpdateText failed: %d\r\n", res);
  }

  res = SD_ReadBack(SD_TEST_FILENAME, readBuf, sizeof(readBuf));
  if (res == FR_OK)
  {
    printf("After update: %s\r\n", readBuf);
  }

  /* SD_DeleteFile(SD_TEST_FILENAME); */ /* left disabled, see SD_DeleteFile() note above */

  res = SD_Unmount();
  if (res != FR_OK)
  {
    printf("SD_Unmount failed: %d\r\n", res);
  }
}

/* USER CODE END SD_RunTest */
