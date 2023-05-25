/*
 * I2CDriver.c
 *
 *  Created on: May 11, 2023
 *      Author: isabel
 */


#include <stdint.h>
#include "I2CDriver.h"




void i2c_config(I2C_Handler_t *ptrHandlerI2C){

	// 1. Activar la señal de reloj para el I2C seleccionado
	if(ptrHandlerI2C->ptrI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	//2. Reiniciar el periférico de forma que inicia en un estado conocido
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_SWRST;


	/*
	 * 3. Indicamos cual es la velocidad del reloj princial, que es la señal
	 * utilizada por el periférico para generar la señal de reloj para el bus I2C
	 */
	ptrHandlerI2C->ptrI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos);

	if(ptrHandlerI2C->Clock_Freq == CLOCK_FREQ_16MHz){
		ptrHandlerI2C->ptrI2Cx->CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C << I2C_CR2_FREQ_Pos);
	}
	else if(ptrHandlerI2C->Clock_Freq == CLOCK_FREQ_80MHz){
		ptrHandlerI2C->ptrI2Cx->CR2 |= (MAIN_CLOCK_40MHz_FOR_I2C << I2C_CR2_FREQ_Pos);
	}


	/*
	 * 4. Configuramos el modo I2C en el que el sistema funciona.
	 * En esta configuración se incluye la velocidad del reloj y
	 * el tiempo máximo para el cambio de la señal (Trise)
	 * Se comienza con los registros en 0
	 */
	ptrHandlerI2C->ptrI2Cx->CCR = 0;
	ptrHandlerI2C->ptrI2Cx->TRISE = 0;

	if(ptrHandlerI2C->modeI2C == I2C_MODE_SM){
		//Seleccionamos el modo estandar
		ptrHandlerI2C->ptrI2Cx->CCR &= ~I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la señal de reloj y el TRISE max
		if(ptrHandlerI2C->Clock_Freq == CLOCK_FREQ_16MHz){
			//Configuramos la señal de reloj cuando el MCU está a 16MHz
			ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);
			//Configuramos el TRISE cuando el MCU está a 16MHz
			ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM;
		}

		else if(ptrHandlerI2C->Clock_Freq == CLOCK_FREQ_80MHz){
			//Configuramos la señal de reloj cuando el MCU está a 80MHz
			ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_80MHZ_SPEED_100KHz << I2C_CCR_CCR_Pos);
			//Configuramos el TRISE cuando el MCU está a 80MHz
			ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_80MHZ_MAX_RISE_TIME_SM;
		}

	}
	else{
		//Configuramos el modo fast
		ptrHandlerI2C->ptrI2Cx->CCR |= I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la reñal de reloj y el TRISE max
		if(ptrHandlerI2C->Clock_Freq == CLOCK_FREQ_16MHz){
			//Configuramos la señal de reloj cuando el MCU está a 16MHz
			ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz << I2C_CCR_CCR_Pos);
			//Configuramos el TRISE cuando el MCU está a 16MHz
			ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM;
		}

		else if(ptrHandlerI2C->Clock_Freq == CLOCK_FREQ_80MHz){
			//Configuramos la señal de reloj cuando el MCU está a 80MHz
			ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_80MHZ_SPEED_400KHz << I2C_CCR_CCR_Pos);
			//Configuramos el TRISE cuando el MCU está a 80MHz
			ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_80MHZ_MAX_RISE_TIME_FM;
		}
	}

	//5. Activamos el módulo I2C
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_PE;

}

//. Generamos la condición de STOP
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C){
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;
}

/*
 * 1. Verificamos que la línea no esté ocupada
 * 2. Generamos la señal de START
 * 2a. Esperamos a que la bandera del evento start se levante
 *
 */

void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C){
	//Verificamos que la línea no esté ocuapda
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	//Generamos la señal start
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	//2a. Esperamos que la bandera del evento start se levante
	//Mientras esperamos, el valor de SB=0, entonces la negación es 1
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}

void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C){
	//Generamos la señal de start
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	//2a. Esperamos que la bandera del evento start se levante
	//Mientras esperamos, el valor de SB=0, entonces la negación es 1
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}

//Activamos la indicación para no-ACK (para el slave de terminar)
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C){
	//Escribir cero en la posición ACK del registro de control 1
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

//Activamos la indicación para ACK
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C){
	//Escribir 1 en la posición ACK del registro de control 1
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_ACK;
}

void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress,uint8_t readOrWrite){
	//Definir variable auxiliar
	uint8_t auxByte = 0;
	(void) auxByte;

	//Enviamos la dirección del Slave y el bit que indica que deseamos escribir (0),
	//Se envía la dirección de mempria que se desea escribir
	ptrHandlerI2C->ptrI2Cx->DR = (slaveAddress << 1) | readOrWrite;

	//Esperamos hasta que la bandera del evento addr se levante
	//Esto indica que la dirección fue enviada satisfactoriamente
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}
	auxByte = ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte = ptrHandlerI2C->ptrI2Cx->SR2;
}

void i2c_sendMemory_Address(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	//Enviamos la dirección de memoria que deseamos leer
	ptrHandlerI2C->ptrI2Cx->DR = memAddr;

	//Esperamos hasta que el bye sea transmitido
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}
}

void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	//Cargamos el valor que deseamos escribir
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;

	//Esperamos hasta que el byte sea transmitido
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
		__NOP();
	}
}

uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C){
	//Esperamos hasta que el byte entrante sea recibido
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE)){
		__NOP();
	}

	ptrHandlerI2C->dataI2C = ptrHandlerI2C->ptrI2Cx->DR;
	return ptrHandlerI2C->dataI2C;
}

uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead){
	//0. Creamos una variable auxiliar para recibir el dato que leemos
	uint8_t auxRead = 0;

	//1. Generamos la condición start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de escribir
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//3. Enviamos la dirección de memoria que deseamos leer
	i2c_sendMemory_Address(ptrHandlerI2C, regToRead);

	//4. Creamos una condición de re-start
	i2c_reStartTransaction(ptrHandlerI2C);

	//5. Enviamos la dirección deñ esclavo y la indicación de leer
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	//8. Leemos el dato que envía el esclavo
	auxRead = i2c_readDataByte(ptrHandlerI2C);

	//6. Generamos la condición de NoACK, para que el master no responda y el slave solo envie 1 byte
	i2c_sendNoAck(ptrHandlerI2C);

	//7. Generamos la condición de Stop, para que el slave se detenga después de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

	return auxRead;
}

void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue){
	//1. Generamos la condición de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de escribir
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//3. Enviamos la dirección de memoria que deseamos escribir
	i2c_sendMemory_Address(ptrHandlerI2C, regToRead);

	//4. Enviamos el valor que deseamos escribir en el registro seleccionado
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	//5. Generamos la condición de STOP para que el valor se detenga despúes de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

}



