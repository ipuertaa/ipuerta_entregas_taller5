/*
 * I2CDriver.h
 *
 *  Created on: May 11, 2023
 *      Author: isabel
 */

#ifndef I2CDRIVER_H_
#define I2CDRIVER_H_

#include <stm32f4xx.h>

#define I2C_WRITE_DATA		0
#define I2C_READ_DATA		1

#define MAIN_CLOCK_4_MHz_FOR_I2C 		4
#define MAIN_CLOCK_16_MHz_FOR_I2C		16
#define MAIN_CLOCK_20MHz_FOR_I2C		20
#define MAIN_CLOCK_40MHz_FOR_I2C		40

#define I2C_MODE_SM		0
#define I2C_MODE_FM		1

#define I2C_MODE_SM_SPEED_100KHz		80
#define I2C_MODE_FM_SPEED_400KHz		14
#define I2C_MODE_SM_80MHZ_SPEED_100KHz	200
#define I2C_MODE_FM_80MHZ_SPEED_400KHz	33


#define I2C_MAX_RISE_TIME_SM			17
#define I2C_MAX_RISE_TIME_FM			5
#define I2C_80MHZ_MAX_RISE_TIME_SM		41
#define I2C_80MHZ_MAX_RISE_TIME_FM		13

#define CLOCK_FREQ_16MHz				1
#define CLOCK_FREQ_80MHz				2


typedef struct
{
	I2C_TypeDef		*ptrI2Cx;
	uint8_t 		slaveAddress;
	uint8_t			modeI2C;
	uint8_t			dataI2C;
	uint8_t			Clock_Freq;
}I2C_Handler_t;

//Prototipos de funciones publicas
void i2c_config(I2C_Handler_t *ptrHandlerI2C);
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress,uint8_t readOrWrite);
void i2c_sendMemory_Address(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr);
void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);
uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C);
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C);

uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead);
void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue);

uint16_t* i2c_readMultipleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t regLimit);

#endif /* I2CDRIVER_H_ */
