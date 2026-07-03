/*
 * mpu6050.c
 *
 *  Created on: Jul 2, 2026
 *      Author: ryzel
 */

/*
 * MPU6050 IMU I2C Driver
 * (Accelerometer + Gyroscope + Temperature)
 *
 * Converted from ADXL355 driver by Philip Salmony
 *
 */

/* USER CODE BEGIN Includes */
#include "MPU6050.h"
/* USER CODE END Includes */

/*
 * INITIALISATION
 */
uint8_t MPU6050_Initialise(MPU6050 *dev, I2C_HandleTypeDef *i2cHandle)
{
    /* USER CODE BEGIN MPU6050_Initialise_0 */

    /* USER CODE END MPU6050_Initialise_0 */

    /* Set struct parameters */
    dev->i2cHandle    = i2cHandle;

    dev->acc_mps2[0]  = 0.0f;
    dev->acc_mps2[1]  = 0.0f;
    dev->acc_mps2[2]  = 0.0f;

    dev->gyro_dps[0]  = 0.0f;
    dev->gyro_dps[1]  = 0.0f;
    dev->gyro_dps[2]  = 0.0f;

    dev->temp_C       = 0.0f;

    /* Store number of transaction errors (to be returned at end of function) */
    uint8_t errNum = 0;
    HAL_StatusTypeDef status;

    /* USER CODE BEGIN MPU6050_Initialise_1 */

    /* USER CODE END MPU6050_Initialise_1 */

    /*
     * Check WHO_AM_I register (Datasheet p. 46)
     * Expected value: 0x68
     */
    uint8_t regData;

    status = MPU6050_ReadRegister(dev, MPU6050_REG_WHO_AM_I, &regData);
    errNum += (status != HAL_OK);

    if (regData != MPU6050_WHO_AM_I_VAL)
    {
        return 255;
    }

    /*
     * Wake up the sensor — clear sleep bit in PWR_MGMT_1 (Datasheet p. 40)
     * Reset value is 0x40 (sleep mode enabled), write 0x00 to wake up
     */
    regData = 0x00;

    status = MPU6050_WriteRegister(dev, MPU6050_REG_PWR_MGMT_1, &regData);
    errNum += (status != HAL_OK);

    /*
     * Configure accelerometer range: +-2g (Datasheet p. 15)
     * ACCEL_CONFIG register: AFS_SEL bits [4:3] = 00 -> +-2g
     * Scale factor: 16384 LSB/g
     */
    regData = 0x00;

    status = MPU6050_WriteRegister(dev, MPU6050_REG_ACCEL_CONFIG, &regData);
    errNum += (status != HAL_OK);

    /*
     * Configure gyroscope range: +-250 deg/s (Datasheet p. 14)
     * GYRO_CONFIG register: FS_SEL bits [4:3] = 00 -> +-250 deg/s
     * Scale factor: 131 LSB/(deg/s)
     */
    regData = 0x00;

    status = MPU6050_WriteRegister(dev, MPU6050_REG_GYRO_CONFIG, &regData);
    errNum += (status != HAL_OK);

    /*
     * Set sample rate divider (Datasheet p. 11)
     * Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
     * With SMPLRT_DIV = 7 and 1kHz gyro rate -> 125 Hz sample rate
     */
    regData = 0x07;

    status = MPU6050_WriteRegister(dev, MPU6050_REG_SMPLRT_DIV, &regData);
    errNum += (status != HAL_OK);

    /* USER CODE BEGIN MPU6050_Initialise_2 */

    /* USER CODE END MPU6050_Initialise_2 */

    /* Return number of errors (0 if successful initialisation) */
    return errNum;
}

/*
 * DATA ACQUISITION
 */

HAL_StatusTypeDef MPU6050_ReadTemperature(MPU6050 *dev)
{
    /* USER CODE BEGIN MPU6050_ReadTemperature_0 */

    /* USER CODE END MPU6050_ReadTemperature_0 */

    /*
     * Read raw temperature registers (16 bits) (Datasheet p. 30)
     */
    uint8_t regData[2];

    HAL_StatusTypeDef status = MPU6050_ReadRegisters(dev, MPU6050_REG_TEMP_OUT_H, regData, 2);

    /*
     * Combine into signed 16-bit raw temperature value
     */
    int16_t tempRaw = (int16_t)((regData[0] << 8) | regData[1]);

    /*
     * Convert to deg C (Datasheet p. 30)
     * Formula: Temp_degC = (TEMP_OUT / 340.0) + 36.53
     */
    dev->temp_C = ((float)tempRaw / 340.0f) + 36.53f;

    /* USER CODE BEGIN MPU6050_ReadTemperature_1 */

    /* USER CODE END MPU6050_ReadTemperature_1 */

    return status;
}

HAL_StatusTypeDef MPU6050_ReadAccelerations(MPU6050 *dev)
{
    /* USER CODE BEGIN MPU6050_ReadAccelerations_0 */

    /* USER CODE END MPU6050_ReadAccelerations_0 */

    /*
     * Read raw accelerometer registers (X, Y, Z -> 16 bits each = 6 bytes total) (Datasheet p. 29)
     */
    uint8_t regData[6];

    HAL_StatusTypeDef status = MPU6050_ReadRegisters(dev, MPU6050_REG_ACCEL_XOUT_H, regData, 6);

    /*
     * Combine register values into signed 16-bit raw accelerometer readings
     */
    int16_t accRaw[3];

    accRaw[0] = (int16_t)((regData[0] << 8) | regData[1]); /* X-axis */
    accRaw[1] = (int16_t)((regData[2] << 8) | regData[3]); /* Y-axis */
    accRaw[2] = (int16_t)((regData[4] << 8) | regData[5]); /* Z-axis */

    /*
     * Convert to m/s^2
     * Scale factor for +-2g range: 16384 LSB/g (Datasheet p. 15)
     * 1g = 9.81 m/s^2
     */
    dev->acc_mps2[0] = 9.81f * ((float)accRaw[0] / 16384.0f);
    dev->acc_mps2[1] = 9.81f * ((float)accRaw[1] / 16384.0f);
    dev->acc_mps2[2] = 9.81f * ((float)accRaw[2] / 16384.0f);

    /* USER CODE BEGIN MPU6050_ReadAccelerations_1 */

    /* USER CODE END MPU6050_ReadAccelerations_1 */

    return status;
}

HAL_StatusTypeDef MPU6050_ReadGyroscope(MPU6050 *dev)
{
    /* USER CODE BEGIN MPU6050_ReadGyroscope_0 */

    /* USER CODE END MPU6050_ReadGyroscope_0 */

    /*
     * Read raw gyroscope registers (X, Y, Z -> 16 bits each = 6 bytes total) (Datasheet p. 31)
     */
    uint8_t regData[6];

    HAL_StatusTypeDef status = MPU6050_ReadRegisters(dev, MPU6050_REG_GYRO_XOUT_H, regData, 6);

    /*
     * Combine register values into signed 16-bit raw gyroscope readings
     */
    int16_t gyroRaw[3];

    gyroRaw[0] = (int16_t)((regData[0] << 8) | regData[1]); /* X-axis */
    gyroRaw[1] = (int16_t)((regData[2] << 8) | regData[3]); /* Y-axis */
    gyroRaw[2] = (int16_t)((regData[4] << 8) | regData[5]); /* Z-axis */

    /*
     * Convert to degrees per second (deg/s)
     * Scale factor for +-250 deg/s range: 131 LSB/(deg/s) (Datasheet p. 14)
     */
    dev->gyro_dps[0] = (float)gyroRaw[0] / 131.0f;
    dev->gyro_dps[1] = (float)gyroRaw[1] / 131.0f;
    dev->gyro_dps[2] = (float)gyroRaw[2] / 131.0f;

    /* USER CODE BEGIN MPU6050_ReadGyroscope_1 */

    /* USER CODE END MPU6050_ReadGyroscope_1 */

    return status;
}

/*
 * LOW-LEVEL FUNCTIONS
 */

HAL_StatusTypeDef MPU6050_ReadRegister(MPU6050 *dev, uint8_t reg, uint8_t *data)
{
    return HAL_I2C_Mem_Read(dev->i2cHandle, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MPU6050_ReadRegisters(MPU6050 *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
    return HAL_I2C_Mem_Read(dev->i2cHandle, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MPU6050_WriteRegister(MPU6050 *dev, uint8_t reg, uint8_t *data)
{
    return HAL_I2C_Mem_Write(dev->i2cHandle, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

