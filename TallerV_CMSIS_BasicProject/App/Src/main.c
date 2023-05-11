/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez

	Solución básica de u  proyecto con librerías externas
 ******************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"


// Definición de los handlers necesarios

GPIO_Handler_t handlerBlinkyLED			= {0};
GPIO_Handler_t handlerUserButton		= {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
USART_Handler_t handlerUsart2			= {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};



//Definición de variables
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;
char bufferMsg[64] = {0};

float valueA = 123.4567f;
float valueB = 987.7654f;
float valueC= 0.0f;


//Definición de funciones
void init_hardware(void);


//Definición de las cabeceras de funciones

//void initSystem(void);

//Función principal del programa

int main(void){

	//Inicializar todos los elementos

	init_hardware();


	while(1){

		if(usart2DataReceived != '\0'){

			valueC = valueA * valueB;
			sprintf(bufferMsg, "ValueC = %#.3f \n", valueC);
			writeMsg(&handlerUsart2, bufferMsg);

			usart2DataReceived = '\0';
		}


	}	//Fin while


}	//Fin main

void init_hardware(void){

	//configurar LED2 - PA5 - LED DE ESTADO

	handlerBlinkyLED.pGPIOx 										= GPIOA;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerBlinkyLED);

	//Configurar el timer para hacer el blinky del led de estado. Lanza interrupción cada 250 ms

	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);

	//Configuración de la comunicación serial:

	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	handlerUsart2.ptrUSARTx = USART2;
	handlerUsart2.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	handlerUsart2.USART_Config.USART_enableIntTX = USART_TX_INTERRUPT_DISABLE;

	USART_Config(&handlerUsart2);
}





//Timer para el LED de estado
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerBlinkyLED);
	sendMsg++;
}

// Función que se ejecuta cada vez que un caracter es recibido
void usart2Rx_Callback(void){
	usart2DataReceived = getRxData();


}



