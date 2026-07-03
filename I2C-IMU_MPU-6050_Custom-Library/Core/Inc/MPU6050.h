/*
 * mpu6050.h
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

#ifndef MPU6050_I2C_DRIVER_H
#define MPU6050_I2C_DRIVER_H

#include "stm32f4xx_hal.h" /* Needed for I2C */

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/*
 * I2C ADDRESS
 * AD0 pin LOW  -> 0x68
 * AD0 pin HIGH -> 0x69
 */
#define MPU6050_I2C_ADDR        (0x68 << 1)

/* WHO_AM_I expected value */
#define MPU6050_WHO_AM_I_VAL    0x68

/* USER CODE BEGIN Defines */

/* USER CODE END Defines */

/*
 * REGISTERS (Datasheet p. 7)
 */
#define MPU6050_REG_SELF_TEST_X     0x0D
#define MPU6050_REG_SELF_TEST_Y     0x0E
#define MPU6050_REG_SELF_TEST_Z     0x0F
#define MPU6050_REG_SELF_TEST_A     0x10
#define MPU6050_REG_SMPLRT_DIV      0x19
#define MPU6050_REG_CONFIG          0x1A
#define MPU6050_REG_GYRO_CONFIG     0x1B
#define MPU6050_REG_ACCEL_CONFIG    0x1C
#define MPU6050_REG_FIFO_EN         0x23
#define MPU6050_REG_INT_ENABLE      0x38
#define MPU6050_REG_INT_STATUS      0x3A
#define MPU6050_REG_ACCEL_XOUT_H    0x3B
#define MPU6050_REG_ACCEL_XOUT_L    0x3C
#define MPU6050_REG_ACCEL_YOUT_H    0x3D
#define MPU6050_REG_ACCEL_YOUT_L    0x3E
#define MPU6050_REG_ACCEL_ZOUT_H    0x3F
#define MPU6050_REG_ACCEL_ZOUT_L    0x40
#define MPU6050_REG_TEMP_OUT_H      0x41
#define MPU6050_REG_TEMP_OUT_L      0x42
#define MPU6050_REG_GYRO_XOUT_H     0x43
#define MPU6050_REG_GYRO_XOUT_L     0x44
#define MPU6050_REG_GYRO_YOUT_H     0x45
#define MPU6050_REG_GYRO_YOUT_L     0x46
#define MPU6050_REG_GYRO_ZOUT_H     0x47
#define MPU6050_REG_GYRO_ZOUT_L     0x48
#define MPU6050_REG_USER_CTRL       0x6A
#define MPU6050_REG_PWR_MGMT_1      0x6B
#define MPU6050_REG_PWR_MGMT_2      0x6C
#define MPU6050_REG_FIFO_COUNTH     0x72
#define MPU6050_REG_FIFO_COUNTL     0x73
#define MPU6050_REG_FIFO_R_W        0x74
#define MPU6050_REG_WHO_AM_I        0x75

/* USER CODE BEGIN Register Defines */

/* USER CODE END Register Defines */

/*
 * SENSOR STRUCT
 */
typedef struct
{
    /* I2C handle */
    I2C_HandleTypeDef *i2cHandle;

    /* Acceleration data (X, Y, Z) in m/s^2 */
    float acc_mps2[3];

    /* Gyroscope data (X, Y, Z) in degrees per second */
    float gyro_dps[3];

    /* Temperature data in deg C */
    float temp_C;

} MPU6050;

/* USER CODE BEGIN Struct */

/* USER CODE END Struct */

/*
 * INITIALISATION
 */
uint8_t MPU6050_Initialise(MPU6050 *dev, I2C_HandleTypeDef *i2cHandle);

/*
 * DATA ACQUISITION
 */
HAL_StatusTypeDef MPU6050_ReadTemperature(MPU6050 *dev);
HAL_StatusTypeDef MPU6050_ReadAccelerations(MPU6050 *dev);
HAL_StatusTypeDef MPU6050_ReadGyroscope(MPU6050 *dev);

/*
 * LOW-LEVEL FUNCTIONS
 */
HAL_StatusTypeDef MPU6050_ReadRegister(MPU6050 *dev, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef MPU6050_ReadRegisters(MPU6050 *dev, uint8_t reg, uint8_t *data, uint8_t length);
HAL_StatusTypeDef MPU6050_WriteRegister(MPU6050 *dev, uint8_t reg, uint8_t *data);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#endif /* MPU6050_I2C_DRIVER_H */
