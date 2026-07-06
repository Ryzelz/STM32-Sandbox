/*
 * mmpu6050.h
 *
 *  Created on: Jul 6, 2026
 *      Author: ryzel
 */

#ifndef MMPU6050_H_
#define MMPU6050_H_
#include "stm32f4xx_hal.h"

//Defines
#define MPU6050_I2C_ADDR 		(0x68 << 1)
#define MPU6050_DEVICE_ID 		(0x68)

//registers
#define MPU6050_REG_SMPLRT_DIV		0x19
#define MPU6050_REG_CONFIG			0x1A
#define MPU6050_REG_GYRO_CONFIG		0x1B
#define MPU6050_REG_ACCEL_CONFIG	0x1C
#define MPU6050_REG_INT_ENABLE		0x38
#define MPU6050_REG_ACCEL_XOUT_H	0x3B
#define MPU6050_REG_ACCEL_XOUT_L	0x3C
#define MPU6050_REG_ACCEL_YOUT_H	0x3D
#define MPU6050_REG_ACCEL_YOUT_L	0x3E
#define MPU6050_REG_ACCEL_ZOUT_H	0x3F
#define MPU6050_REG_ACCEL_ZOUT_L	0x40
#define MPU6050_REG_TEMP_OUT_H		0x41
#define MPU6050_REG_TEMP_OUT_L		0x42
#define MPU6050_REG_GYRO_XOUT_H		0x43
#define MPU6050_REG_GYRO_XOUT_L		0x44
#define MPU6050_REG_GYRO_YOUT_H		0x45
#define MPU6050_REG_GYRO_YOUT_L		0x46
#define MPU6050_REG_GYRO_ZOUT_H		0x47
#define MPU6050_REG_GYRO_ZOUT_L		0x48
#define MPU6050_REG_PWR_MGMT_1		0x6B
#define MPU6050_REG_PWR_MGMT_2		0x6C
#define MPU6050_REG_WHO_AM_I		0x75
/*

#define MPU6050_REG_SMPLRT_DIV 0x19
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_
#define MPU6050_REG_

*/

/*
 * SENSOR STRUCT
 */
typedef struct {
	/* I2C handle */
	I2C_HandleTypeDef *i2cHandle;
	/* Acceleration data (X, Y, Z) in m/s^2 */
	float acc_mps2[3];
	/* Gyroscope data (X, Y, Z) in deg/s */
	float gyro_dps[3];
	/* Temperature data in deg C */
	float temp_C;
} MPU6050;
/*
 * INITIALISATION
 */
uint8_t MPU6050_Initialise( MPU6050 *dev, I2C_HandleTypeDef *i2cHandle );
/*
 * DATA ACQUISITION
 */
HAL_StatusTypeDef MPU6050_ReadTemperature( MPU6050 *dev );
HAL_StatusTypeDef MPU6050_ReadAccelerations( MPU6050 *dev );
HAL_StatusTypeDef MPU6050_ReadGyroscope( MPU6050 *dev );
/*
 * LOW-LEVEL FUNCTIONS
 */
HAL_StatusTypeDef MPU6050_ReadRegister(  MPU6050 *dev, uint8_t reg, uint8_t *data );
HAL_StatusTypeDef MPU6050_ReadRegisters( MPU6050 *dev, uint8_t reg, uint8_t *data, uint8_t length );
HAL_StatusTypeDef MPU6050_WriteRegister( MPU6050 *dev, uint8_t reg, uint8_t *data );

#endif /* MMPU6050_H_ */
