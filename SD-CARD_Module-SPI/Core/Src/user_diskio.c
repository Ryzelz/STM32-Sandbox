/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file bridges FatFs to the custom sd_driver_* SPI functions.
  ******************************************************************************
  */
/* USER CODE END Header */

/* USER CODE BEGIN DECL */
#include <string.h>
#include "ffconf.h"
#include "diskio.h"
#include "spi.h"
#include "sd_driver_init.h"
#include "sd_driver_read.h"
#include "sd_driver_write.h"

#define SD_BLOCK_SIZE 512U

/* USER CODE END DECL */

/* Private function prototypes ----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif
#if _USE_IOCTL == 1
DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif

Diskio_drvTypeDef USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if _USE_WRITE == 1
  USER_write,
#endif
#if _USE_IOCTL == 1
  USER_ioctl,
#endif
};

/* USER CODE BEGIN PV */
static volatile DSTATUS sd_disk_status = STA_NOINIT;
/* USER CODE END PV */

/**
  * @brief  Initializes the SD card and reports disk status
  * @param  pdrv Physical drive number
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize(BYTE pdrv)
{
  /* USER CODE BEGIN INIT */
  if (pdrv != 0)
    return STA_NOINIT;

  if (sd_card_reset(&hspi2, false) != SD_OK)
  {
    sd_disk_status = STA_NOINIT;
    return sd_disk_status;
  }

  sd_disk_status &= ~STA_NOINIT;
  return sd_disk_status;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets disk status
  * @param  pdrv Physical drive number
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status(BYTE pdrv)
{
  /* USER CODE BEGIN STATUS */
  if (pdrv != 0)
    return STA_NOINIT;

  return sd_disk_status;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads sector(s)
  * @param  pdrv Physical drive number
  * @param  buff Data buffer to store read data
  * @param  sector Start sector number
  * @param  count Number of sectors to read
  * @retval DRESULT: Operation result
  */
DRESULT USER_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
  /* USER CODE BEGIN READ */
  if (pdrv != 0)
    return RES_PARERR;

  // sd_driver addressing is byte-based; multiply by block size
  sd_error status = sd_card_read_multiple_data(
    &hspi2, sector * SD_BLOCK_SIZE, buff, SD_BLOCK_SIZE, count
  );

  return (status == SD_OK) ? RES_OK : RES_ERROR;
  /* USER CODE END READ */
}

/**
  * @brief  Writes sector(s)
  * @param  pdrv Physical drive number
  * @param  buff Data to be written
  * @param  sector Start sector number
  * @param  count Number of sectors to write
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
  /* USER CODE BEGIN WRITE */
  if (pdrv != 0)
    return RES_PARERR;

  sd_error status = sd_card_write_multiple_data(
    &hspi2, sector * SD_BLOCK_SIZE, buff, SD_BLOCK_SIZE, count
  );

  return (status == SD_OK) ? RES_OK : RES_ERROR;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv Physical drive number
  * @param  cmd Control code
  * @param  buff Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
  /* USER CODE BEGIN IOCTL */
  DRESULT res = RES_ERROR;

  if (pdrv != 0)
    return RES_PARERR;

  switch (cmd)
  {
    case CTRL_SYNC:
      res = RES_OK;
      break;

    case GET_SECTOR_COUNT:
    {
      sd_info info = { 0 };
      if (sd_card_get_common_info(&hspi2, &info) != SD_OK)
        return RES_ERROR;
      *(DWORD *)buff = info.size / SD_BLOCK_SIZE;
      res = RES_OK;
      break;
    }

    case GET_SECTOR_SIZE:
      *(WORD *)buff = SD_BLOCK_SIZE;
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE:
      *(DWORD *)buff = 1; // Erase block size in units of sectors (unknown, default 1)
      res = RES_OK;
      break;

    default:
      res = RES_PARERR;
  }

  return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */
