/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez

	Solución básica de u  proyecto con librerías externas
 ******************************************************************************
 */

#include <stm32f4xx.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "PLL.h"




GPIO_Handler_t handlerMCO1 ={0};
//Elementos para la comunicación I2C
GPIO_Handler_t handlerLedOK = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
uint8_t rxData = 0;

BasicTimer_Handler_t handlerStateOKTimer = {0};

USART_Handler_t handlerCommTerminal = {0};
char bufferData[64] = "Accel MPU-6050 testing";

uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint8_t systemTicksEnd = 0;

GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

char mensajePrueba[100] = {0};
uint8_t dia = 24;
uint8_t mes = 5;
uint8_t anio = 23;

#define ACCEL_ADDRESS	0b1101001;
#define ACCEL_XOUT_H	59
#define ACCEL_XOUT_L	60
#define ACCEL_YOUT_H	61
#define ACCEL_YOUT_L	62
#define ACCEL_ZOUT_H	63
#define ACCEL_ZOUT_L	64

#define PWR_MGMT_1 107
#define WHO_AM_I 117


//MIN 19

//Definición de funciones
void init_hardware(void);


//Función principal del programa

int main(void){

	//Activar el co-procesador
//	SCB->CPACR |= (0xF << 20);

	//Inicializar todos los elementos
	init_hardware();
//	configPLL80MHz();




	//Imprimir un mensaje de inicio




	while(1){

		if(rxData != '\0'){
//			writeChar(&handlerCommTerminal, rxData);
			sprintf(mensajePrueba, "\nMensaje a enviar %d %d %d", dia, mes, anio);
			writeMsg(&handlerCommTerminal, mensajePrueba);
			rxData = '\0';



		}


		//Hacemos un "eco" con el valor que nos llega por el serial
//
//		if(rxData != '\0'){
//			writeChar(&handlerCommTerminal, rxData);
//
//			if(rxData == 'w'){
//				sprintf(bufferData, "WHO_AM_I? (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//
//				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
//				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//			}
//			else if(rxData == 'p'){
//
//				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//
//				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
//				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = 0;
//
//			}
//			else if(rxData == 'r'){
//				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//
//				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
//				rxData = '\0';
//			}
//
//			else if(rxData == 'x'){
//				sprintf(bufferData, "Axis X data (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//
//				uint8_t accelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
//				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
//				int16_t AccelX = AccelX_high << 8 | accelX_low;
//				sprintf(bufferData, "AccelX = %d \n", (int)AccelX);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//
//			}
//			else if(rxData == 'y'){
//				sprintf(bufferData, "Axis Y data (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
//				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
//				int16_t AccelY = AccelY_high << 8 | AccelY_low;
//				sprintf(bufferData, "AccelY = %d \n", (int)AccelY);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//			}
//			else if(rxData == 'z'){
//				sprintf(bufferData, "Axis Z data(r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
//				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
//				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
//				sprintf(bufferData, "AccelZ = %d \n", (int)AccelZ);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//			}
//			else {
//				rxData = '\0';
//			}
//
//
//
//		}



	}	//Fin while



}	//Fin main

void init_hardware(void){

	//configurar LED2 - PA5 - LED DE ESTADO

	handlerLedOK.pGPIOx 										= GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedOK);

	//Configuración de la comunicación serial:

	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	handlerCommTerminal.ptrUSARTx = USART2;
	handlerCommTerminal.USART_Config.USART_baudrate = USART_BAUDRATE_19200;
	handlerCommTerminal.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX = USART_TX_INTERRUPT_ENABLE;

	USART_Config(&handlerCommTerminal);

	handlerStateOKTimer.ptrTIMx  = TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerStateOKTimer.TIMx_Config.TIMx_period = 250;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable = 1;

	BasicTimer_Config(&handlerStateOKTimer);


	//Pines con los que funciona el I2C

//	handlerI2cSCL.pGPIOx = GPIOB;
//	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
//	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
//	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
//	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
//
//	GPIO_Config(&handlerI2cSCL);
//
//	handlerI2cSDA.pGPIOx	= GPIOB;
//	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
//	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
//	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
//	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
//
//	GPIO_Config(&handlerI2cSDA);
//
//	handlerAccelerometer.ptrI2Cx = I2C1;
//	handlerAccelerometer.modeI2C = I2C_MODE_FM;
//	handlerAccelerometer.slaveAddress = ACCEL_ADDRESS;
//
//	i2c_config(&handlerAccelerometer);


	//configurar el pin para medir la señal de reloj del micro

//	handlerMCO1.pGPIOx 										= GPIOA;
//	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_8;
//	handlerMCO1.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
//	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
//	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
//	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
//
//	GPIO_Config(&handlerMCO1);



}





//Callback para la recepción del USART2

void usart2Rx_Callback(void){
	rxData = getRxData();
}

//Callback para la transmisión del USART2
void usart2Tx_Callback(void){

}


void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLedOK);
}



