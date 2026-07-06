/*
 * MPU6050.h
 *
 *  Created on: Jul 6, 2026
 *      Author: ryzel
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "stm32f4xx_hal.h" /* Needed for I2C */

/*
 * DEFINES
 */
#define MPU6050_I2C_ADDR		(0x68 << 1) /* AD0 = 0 -> 0x68, AD0 = 1 -> 0x69 */
#define MPU6050_DEVICE_ID		0x68

/*
 * REGISTERS (RM-MPU-6000A-00, Register Map)
 */
#define MPU6050_REG_SMPLRT_DIV		0x19
#define MPU6050_REG_CONFIG			0x1A
#define MPU6050_REG_GYRO_CONFIG		0x1B
#define MPU6050_REG_ACCEL_CONFIG	0x1C
#define MPU6050_REG_INT_ENABLE		0x38
#define MPU6050_REG_ACCEL_XOUT_H	0x3B
#define MPU6050_REG_TEMP_OUT_H		0x41
#define MPU6050_REG_GYRO_XOUT_H		0x43
#define MPU6050_REG_PWR_MGMT_1		0x6B
#define MPU6050_REG_PWR_MGMT_2		0x6C
#define MPU6050_REG_WHO_AM_I		0x75

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

#endif /* INC_MPU6050_H_ */
