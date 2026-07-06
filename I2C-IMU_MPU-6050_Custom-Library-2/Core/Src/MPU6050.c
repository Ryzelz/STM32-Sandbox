/*
 * MPU6050.c
 *
 *  Created on: Jul 6, 2026
 *      Author: ryzel
 */

#include "MPU6050.h"

uint8_t MPU6050_Initialise( MPU6050 *dev, I2C_HandleTypeDef *i2cHandle ) {

	/* Set struct parameters */
	dev->i2cHandle 		= i2cHandle;

	dev->acc_mps2[0]	= 0.0f;
	dev->acc_mps2[1]	= 0.0f;
	dev->acc_mps2[2]	= 0.0f;

	dev->gyro_dps[0]	= 0.0f;
	dev->gyro_dps[1]	= 0.0f;
	dev->gyro_dps[2]	= 0.0f;

	dev->temp_C			= 0.0f;

	/* Store number of transaction errors (to be returned at end of function) */
	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	/*
	 * Check WHO_AM_I register (REGISTER MAP PAGE 46)
	 */
	uint8_t regData;

	status = MPU6050_ReadRegister( dev, MPU6050_REG_WHO_AM_I, &regData );
	errNum += ( status != HAL_OK );

	if ( regData != MPU6050_DEVICE_ID ) {

		return 255;

	}

	/*
	 * Wake sensor up from sleep mode and select PLL with X-axis gyro reference as clock source (REGISTER MAP PAGE 40)
	 */
	regData = 0x01;

	status = MPU6050_WriteRegister( dev, MPU6050_REG_PWR_MGMT_1, &regData );
	errNum += ( status != HAL_OK );

	/*
	 * Set sample rate divider (Sample Rate = Gyro Output Rate / (1 + SMPLRT_DIV)) (REGISTER MAP PAGE 11)
	 */
	regData = 0x07;

	status = MPU6050_WriteRegister( dev, MPU6050_REG_SMPLRT_DIV, &regData );
	errNum += ( status != HAL_OK );

	/*
	 * Set digital low-pass filter (DLPF_CFG = 1 -> 184 Hz accel / 188 Hz gyro bandwidth) (REGISTER MAP PAGE 13)
	 */
	regData = 0x01;

	status = MPU6050_WriteRegister( dev, MPU6050_REG_CONFIG, &regData );
	errNum += ( status != HAL_OK );

	/*
	 * Set gyroscope full scale range (+-250 deg/s) (REGISTER MAP PAGE 14)
	 */
	regData = 0x00;

	status = MPU6050_WriteRegister( dev, MPU6050_REG_GYRO_CONFIG, &regData );
	errNum += ( status != HAL_OK );

	/*
	 * Set accelerometer full scale range (+-2g) (REGISTER MAP PAGE 15)
	 */
	regData = 0x00;

	status = MPU6050_WriteRegister( dev, MPU6050_REG_ACCEL_CONFIG, &regData );
	errNum += ( status != HAL_OK );

	/* Return number of errors (0 if successful initialisation) */
	return errNum;

}

/*
 * DATA ACQUISITION
 */

HAL_StatusTypeDef MPU6050_ReadTemperature( MPU6050 *dev ) {

	/* REGISTER MAP PAGE 30 */

	/*
	 * Read raw values from temperature registers (16 bits)
	 */
	uint8_t regData[2];

	HAL_StatusTypeDef status = MPU6050_ReadRegisters( dev, MPU6050_REG_TEMP_OUT_H, regData, 2 );

	/*
	 * Combine register values to give raw (SIGNED) temperature reading
	 */
	int16_t tempRaw = (int16_t) ( (regData[0] << 8) | regData[1] );

	/*
	 * Convert to deg C (Temperature in degrees C = (TEMP_OUT Register Value)/340 + 36.53)
	 */
	dev->temp_C = ( (float) tempRaw / 340.0f ) + 36.53f;

	return status;

}

HAL_StatusTypeDef MPU6050_ReadAccelerations( MPU6050 *dev ) {

	/* REGISTER MAP PAGE 29 */

	/*
	 * Read raw values from acceleration registers (x, y, z -> 16 bits each)
	 */
	uint8_t regData[6];

	HAL_StatusTypeDef status = MPU6050_ReadRegisters( dev, MPU6050_REG_ACCEL_XOUT_H, regData, 6 );

	/*
	 * Combine register values to give raw (SIGNED) accelerometer readings
	 */
	int16_t accRaw[3];

	accRaw[0] = (int16_t) ( (regData[0] << 8) | regData[1] ); /* X-axis */
	accRaw[1] = (int16_t) ( (regData[2] << 8) | regData[3] ); /* Y-axis */
	accRaw[2] = (int16_t) ( (regData[4] << 8) | regData[5] ); /* Z-axis */

	/* Convert to mps^2 (given range setting of +-2g -> 16384 LSB/g) */
	dev->acc_mps2[0] = 9.81f * ( (float) accRaw[0] / 16384.0f );
	dev->acc_mps2[1] = 9.81f * ( (float) accRaw[1] / 16384.0f );
	dev->acc_mps2[2] = 9.81f * ( (float) accRaw[2] / 16384.0f );

	return status;

}

HAL_StatusTypeDef MPU6050_ReadGyroscope( MPU6050 *dev ) {

	/* REGISTER MAP PAGE 31 */

	/*
	 * Read raw values from gyroscope registers (x, y, z -> 16 bits each)
	 */
	uint8_t regData[6];

	HAL_StatusTypeDef status = MPU6050_ReadRegisters( dev, MPU6050_REG_GYRO_XOUT_H, regData, 6 );

	/*
	 * Combine register values to give raw (SIGNED) gyroscope readings
	 */
	int16_t gyroRaw[3];

	gyroRaw[0] = (int16_t) ( (regData[0] << 8) | regData[1] ); /* X-axis */
	gyroRaw[1] = (int16_t) ( (regData[2] << 8) | regData[3] ); /* Y-axis */
	gyroRaw[2] = (int16_t) ( (regData[4] << 8) | regData[5] ); /* Z-axis */

	/* Convert to deg/s (given range setting of +-250 deg/s -> 131 LSB/(deg/s)) */
	dev->gyro_dps[0] = (float) gyroRaw[0] / 131.0f;
	dev->gyro_dps[1] = (float) gyroRaw[1] / 131.0f;
	dev->gyro_dps[2] = (float) gyroRaw[2] / 131.0f;

	return status;

}

/*
 * LOW-LEVEL FUNCTIONS
 */

HAL_StatusTypeDef MPU6050_ReadRegister( MPU6050 *dev, uint8_t reg, uint8_t *data ) {

	return HAL_I2C_Mem_Read( dev->i2cHandle, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY );

}

HAL_StatusTypeDef MPU6050_ReadRegisters( MPU6050 *dev, uint8_t reg, uint8_t *data, uint8_t length ) {

	return HAL_I2C_Mem_Read( dev->i2cHandle, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY );

}

HAL_StatusTypeDef MPU6050_WriteRegister( MPU6050 *dev, uint8_t reg, uint8_t *data ) {

	return HAL_I2C_Mem_Write( dev->i2cHandle, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY );

}
