/*
 * Sln_examen.c
 *
 *  Created on: Jun 2, 2023
 *      Author: isabel
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
#include "AdcDriver.h"


// Elementos para el led de estado PH0 + TIM2
BasicTimer_Handler_t handlerStateOKTimer = {0};
GPIO_Handler_t handlerLedOK = {0};


// Elementos para la comunicación serial USART1 y el manejo de comandos
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;
char bufferRx[100];
uint16_t counterRx = 0;
uint8_t stringRxComplete = 0;
char comando[64];
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
char userMsg[100];

// Elementos para la conversión analogo digital
ADC_Config_t handlerChannel_0 = {0};
ADC_Config_t handlerChannel_1 = {0};

// Elementos para el MOC1
GPIO_Handler_t handlerMCO1 = {0};


//Definición de cabeceras de funciones
void identificarComandos (char *ptrbufferRx);
void init_hardware(void);


int main(void){

	configPLL(PLL_FRECUENCIA_100MHZ);
	init_hardware();
	writeMsg(&handlerCommTerminal, "Examen Taller 5 Isabel Puerta Alvarez");
	writeMsg(&handlerCommTerminal, "\nSistema inicializado con exito\n");

	configMCO1(CLOCK_SIGNAL_PLL, MCO1_PRESCALERX5);


	while(1){

		if(rxData != '\0'){


			//Se comienza a almacenar la cadena de caracteres recibida
			bufferRx[counterRx] = rxData;
			counterRx++;

			//Se evalúa el final de la cadena de caracteres
			if(rxData == '@'){
				stringRxComplete = 1;

				//Se agrega el caracter nulo al final del string
				bufferRx[counterRx] = '\0';

				// Se detiene el contador
				counterRx = 0;
			}

			//Se detiene el almacenamiento
			rxData = '\0';
		}	//Fin if rxData != 0


		//Se analizan los caracteres obtenidos
		if(stringRxComplete == 1){
			identificarComandos(bufferRx);
			stringRxComplete = 0;
		}

	}	//Fin while
}	//Fin main




















































void identificarComandos (char *ptrbufferRx){

	/*
	 * Lee la cadena de caracteres
	 * Mediante la función sscanf (opuesta al sprintf) se divide la cadena
	 * de caracteres en diferentos elementos
	 * Esto es para recibir frases completas desde el puerto serial
	 */

	sscanf(ptrbufferRx, "%s %u %u %s" , comando, &firstParameter, &secondParameter, userMsg);

	if(strcmp(comando, "help") == 0){
		writeMsg(&handlerCommTerminal, "Menú ayuda. Información sobre los comandos:\n");
	}

}	//FIn identificarComandos



void init_hardware(void){

	/*
	 * 			Para poder utilizar el pin PH0 como led de estado
	 *
	 * En los pines PH0 y PH1 usualmente se encuentran las entradas y salidas del oscilador HSE
	 * Para poder usarlos como GPIO, es necesario apagar el oscilador HSE (después de reset, se
	 * mantiene apagado)
	 *
	 * En el RCC_RC:
	 * 		- HSEON
	 *
	 *
	 */
	RCC->CR &= ~RCC_CR_HSEON;

	//configurar LED2 - PH1 - LED DE ESTADO

	handlerLedOK.pGPIOx 										= GPIOH;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber 					= PIN_1;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedOK);

	handlerStateOKTimer.ptrTIMx  								= TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed 					= BTIMER_SPEED_500us_100MHz;
	handlerStateOKTimer.TIMx_Config.TIMx_period 				= 500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable 		= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerStateOKTimer);


	//Configuración de la comunicación serial USART1

	handlerPinTX.pGPIOx 								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx 								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerPinRX);

	handlerCommTerminal.ptrUSARTx 						= USART1;
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_19200;
	handlerCommTerminal.USART_Config.USART_datasize 	= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_mode 		= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_enableIntRX 	= USART_RX_INTERRUPT_ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX 	= USART_TX_INTERRUPT_ENABLE;

	USART_Config(&handlerCommTerminal);

	//Configuración del pin MOC1

//	//Limpiar los bits correspondientes == Seleccionar HSI como salida
//	RCC->CFGR &= ~(RCC_CFGR_MCO1);
//
//	//Limpiar los bits del prescaler == no escalar
//	RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);

	//Configurar el pin PA8 con la función alternativa AF0

	handlerMCO1.pGPIOx							= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber 	= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed 	= GPIO_OSPEED_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;

	GPIO_Config(&handlerMCO1);

}

// Callback para el led de estado

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLedOK);
}

// Callback para la comunicación serial USART1

// Callback para la recepción del USART1
void usart1Rx_Callback(void){
	//Se lee el dato del DR y se almacena. Al hacer esto se baja la bandera de la interrupción
	rxData = getRxData();
}

//Callback para la transmisión del USART1
void usart1Tx_Callback(void){

}

