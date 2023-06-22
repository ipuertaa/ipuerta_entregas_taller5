/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez

	Programación del teclado matricial
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
#include "SysTickDriver.h"
#include "OledDriver.h"
#include "I2CDriver.h"





/* 4 pines de propósito general configurados como columnas serán salidas a un nivel alto.
 * 4 filas serán configuradas como entradas y serán las que causarán la interrupción.
 *
 */

//Definición de variables

uint8_t banderaBarrido = 0;
uint8_t auxbanderaBarrido = 0;
uint8_t banderaCondicion = 0;
uint8_t flagFilas  = 0;
uint8_t flagMoneda = 0;
uint16_t dinero = 0;

#define PRECIO_CELDA1  1000
#define PRECIO_CELDA2  2500
#define PRECIO_CELDA3  500
#define PRECIO_CELDA4  3500
#define oLEDAddress 			0x3C

char boton = 10;
char casilla[2] = {0};
uint8_t llenarCasilla = 0;
uint8_t casillaCompleta = 0;
uint8_t casilla1OK = 0;



//Definición de los handler

//Handlers del GPIO: Todos los pines, independientes de entrada o salida, se deben configurar

GPIO_Handler_t handlerColumna1 = {0};		//1,4,7,*
GPIO_Handler_t handlerColumna2 = {0};		//2,5,8,0
GPIO_Handler_t handlerColumna3 = {0};		//3,6,9,#
GPIO_Handler_t handlerColumna4 = {0};		//A,B,C,D

GPIO_Handler_t handlerFila1 = {0};			//1,2,3
GPIO_Handler_t handlerFila2 = {0};			//4,5,6
GPIO_Handler_t handlerFila3 = {0};			//7,8,9
GPIO_Handler_t handlerFila4 = {0};			//*,0,#

GPIO_Handler_t handlerLED2 					= {0};

//Handler para el timer: con esto podré controlar el barrido de las columnas

BasicTimer_Handler_t handlerBarrido = {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};

//Handlers para los EXTI. Cada fila es una entrada por donde podrá haber una interrupción o no

EXTI_Config_t handlerEXTI1 = {0};		//exti para la fila 1
EXTI_Config_t handlerEXTI2 = {0};		//exti para la fila 2
EXTI_Config_t handlerEXTI3 = {0};		//exti para la fila 3
EXTI_Config_t handlerEXTI4 = {0};		//exti para la fila 4


//Handler para los elementos asociados a la recepción de las monedas
GPIO_Handler_t handlerRecepMonedas = {0};
EXTI_Config_t handlerEXTI5_monedas = {0}; 	//PB5

//Elementos para la comunicación serial
USART_Handler_t handlerUsart2			= {0};
GPIO_Handler_t handlerPinTX 			= {0};
GPIO_Handler_t handlerPinRX 			= {0};
uint8_t usart2DataReceived = 0;
char bufferMsg[100] = {0};

//Elementos para el manejo de la pantalla OLED
GPIO_Handler_t handlerOledSDA = {0};
GPIO_Handler_t handlerOledSCL = {0};
I2C_Handler_t handlerOLED = {0};

char dummyMsg[64] = {0};

// Cabeceras de funciones

void init_hardware(void);		//Función para la configuración de pines.
char identificarF1(void);
char identificarF2(void);
char identificarF3(void);
char identificarF4(void);
void limpiar_casilla(void);

int main(void){

	init_hardware();
	delay_ms(10);
	initOled(&handlerOLED);
	delay_ms(10);
	sprintf(bufferMsg, "  BIENVENIDO       OPRIMA BOTON");
	OLED_print_msg(&handlerOLED, bufferMsg);


	while(1){

		if(boton != 10){

//			sprintf(bufferMsg, "BOTON");
//			OLED_print_msg_pag_inicio(&handlerOLED, 2, bufferMsg, 0);
			switch (boton){
			case 1:{
				clearScreenOLED(&handlerOLED);
				delay_ms(1);
				sprintf(dummyMsg, "%u", boton);
//				OLED_print_msg_pag(&handlerOLED, 2, dummyMsg);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 2:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 3:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 4:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 5:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 6:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}

			case 7:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 8:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 9:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			case 0:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "%u", boton);
				OLED_print_msg_pag_inicio(&handlerOLED, 2, dummyMsg, 7);
				break;
			}
			default:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "ERROR DEFAULT");
				OLED_print_msg(&handlerOLED, dummyMsg);
				break;
			}

			}




			boton = 10;
//			delay_ms(30);

		}	//Fin if boton




		if(flagMoneda == 1){
			sprintf(bufferMsg, "\nDINERO ingresado = %u $\n", dinero);
			writeMsg(&handlerUsart2, bufferMsg);
			flagMoneda = 0;

		}

		if(banderaCondicion == 0){
			switch(banderaBarrido){
			case 1:{
				GPIO_WritePin(&handlerColumna1, SET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, RESET);
//				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;
			}
			case 2: {
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, SET);
				GPIO_WritePin(&handlerColumna3, RESET);
//				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;
			}
			case 3:{
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, SET);
//				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				banderaBarrido = 0;
				break;

			}
//			case 4:{
//				GPIO_WritePin(&handlerColumna1, RESET);
//				GPIO_WritePin(&handlerColumna2, RESET);
//				GPIO_WritePin(&handlerColumna3, RESET);
//				GPIO_WritePin(&handlerColumna4, SET);
//				banderaCondicion = 1;
//				banderaBarrido = 0;
//				break;
//			}
			default:{
				__NOP();
			}


			} //Fin switch
		} 	//Fin Barrido de columnas

		else{
			__NOP();
		}


	}	//Fin while

}	//Fin main



//		DEFINICIÓN DE LAS FUNCIONES

void init_hardware(void){

	//Configurar la C1

	handlerColumna1.pGPIOx 								= GPIOA;
	handlerColumna1.GPIO_PinConfig.GPIO_PinNumber 		= PIN_15;
	handlerColumna1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna1.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna1.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna1);		//Cargar la configuración
	GPIO_WritePin(&handlerColumna1, RESET);


	//Configurar la C2 -> PB0

	handlerColumna2.pGPIOx 								= GPIOB;
	handlerColumna2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_0;
	handlerColumna2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna2);		//Cargar la configuración
	GPIO_WritePin(&handlerColumna2, RESET);


	//Configurar la C3 -> PC13

	handlerColumna3.pGPIOx 								= GPIOC;
	handlerColumna3.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerColumna3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna3.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna3.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna3);		//Cargar la configuración
	GPIO_WritePin(&handlerColumna3, RESET);

//	//Configurar la C4 -> PC2
//
//	handlerColumna4.pGPIOx 								= GPIOC;
//	handlerColumna4.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
//	handlerColumna4.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
//	handlerColumna4.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
//	handlerColumna4.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
//	handlerColumna4.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
//
//	GPIO_Config(&handlerColumna4);		//Cargar la configuración


	handlerLED2.pGPIOx 										= GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLED2);

	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);

	//Configurar el TIM4 para hacer el barrido

	handlerBarrido.ptrTIMx 							= TIM4;
	handlerBarrido.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerBarrido.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerBarrido.TIMx_Config.TIMx_period 			= 4;
	handlerBarrido.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBarrido);

/*  CONFIGURACIÓN DE LAS FILAS   */

	//Configuración del pin para la F1 ->PC3

	handlerFila1.pGPIOx 								= GPIOC;
	handlerFila1.GPIO_PinConfig.GPIO_PinNumber 			= PIN_3;
	handlerFila1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila1.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI1.pGPIOHandler = &handlerFila1;
	handlerEXTI1.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F1
	extInt_Config(&handlerEXTI1);


	//Configuración del pin para la F2 ->PA4

	handlerFila2.pGPIOx 								= GPIOA;
	handlerFila2.GPIO_PinConfig.GPIO_PinNumber 			= PIN_4;
	handlerFila2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila2.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI2.pGPIOHandler = &handlerFila2;
	handlerEXTI2.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F2
	extInt_Config(&handlerEXTI2);



	//Configuración del pin para la F3 ->PC1

	handlerFila3.pGPIOx 								= GPIOC;
	handlerFila3.GPIO_PinConfig.GPIO_PinNumber 			= PIN_1;
	handlerFila3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila3.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI3.pGPIOHandler = &handlerFila3;
	handlerEXTI3.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F3
	extInt_Config(&handlerEXTI3);


	//Configuración del pin para la F4 ->PC0

	handlerFila4.pGPIOx 								= GPIOC;
	handlerFila4.GPIO_PinConfig.GPIO_PinNumber 			= PIN_0;
	handlerFila4.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila4.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila4.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI4.pGPIOHandler = &handlerFila4;
	handlerEXTI4.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F4
	extInt_Config(&handlerEXTI4);


	/*  CONFIGURACIÓN DE LOS ELEMENTOS ASOCIADOS A LA RECEPCIÓN DE LAS MONEDAS */

	handlerRecepMonedas.pGPIOx 									= GPIOB;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	//Configuración del EXTI

	handlerEXTI5_monedas.pGPIOHandler = &handlerRecepMonedas;
	handlerEXTI5_monedas.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F1
	extInt_Config(&handlerEXTI5_monedas);

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

	handlerUsart2.ptrUSARTx 						= USART2;
	handlerUsart2.USART_Config.USART_baudrate 		= USART_BAUDRATE_19200;
	handlerUsart2.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_mode 			= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_enableIntRX 	= USART_RX_INTERRUPT_ENABLE;
	handlerUsart2.USART_Config.USART_enableIntTX 	= USART_TX_INTERRUPT_ENABLE;

	USART_Config(&handlerUsart2);

	//Inicialización del periférico SysTick
	config_SysTick_ms(HSI_CLOCK_CONFIGURED);

	// Configurar los elementos para el manejo de la pantalla OLED

	handlerOledSCL.pGPIOx									= GPIOB;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinOPType   			= GPIO_OTYPE_OPENDRAIN;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_PULLUP;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerOledSCL);

	handlerOledSDA.pGPIOx 								= GPIOB;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinOPType   		= GPIO_OTYPE_OPENDRAIN;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLUP;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinAltFunMode	= AF4;

	GPIO_Config(&handlerOledSDA);


	handlerOLED.ptrI2Cx				= I2C1;
	handlerOLED.modeI2C				= I2C_MODE_FM;
	handlerOLED.slaveAddress		= oLEDAddress;
	handlerOLED.Clock_Freq			= CLOCK_FREQ_16MHz;

	i2c_config(&handlerOLED);


}


void BasicTimer4_Callback(void){
	banderaBarrido++;
	banderaCondicion = 0;

}

void callback_extInt3(void){	// F1
	boton = identificarF1();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void callback_extInt4(void){	//F2
	boton = identificarF2();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void callback_extInt1(void){	//F3
	boton = identificarF3();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void callback_extInt0(void){	//F4
	boton = identificarF4();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}

void callback_extInt5(void){	//Monedas
	flagMoneda = 1;
	dinero = dinero + 500;

}

// Función que se ejecuta cada vez que un caracter es recibido
void usart2Rx_Callback(void){
	usart2DataReceived = getRxData();

}


char identificarF1(void){
	switch(banderaBarrido){
	case 1:{
		return 1;
		break;
	}
	case 2:{
		return 2;
		break;
	}
	case 3:{
		return 3;
		break;
	}
//	case 4:{
//		return 'A';
//		break;
//	}
	default:{
		return 0;
		break;
	}
	}
}

char identificarF2(void){
	switch(banderaBarrido){
	case 1:{
		return 4;
		break;
	}
	case 2:{
		return 5;
		break;
	}
	case 3:{
		return 6;
		break;
	}
//	case 4:{
//		return 'B';
//		break;
//	}
	default:{
		return 0;
		break;
	}
	}
}

char identificarF3(void){
	switch(banderaBarrido){
	case 1:{
		return 7;
		break;
	}
	case 2:{
		return 8;
		break;
	}
	case 3:{
		return 9;
		break;
	}
//	case 4:{
//		return 'C';
//		break;
//	}
	default:{
		return 0;
		break;
	}
	}
}

char identificarF4(void){
	switch(banderaBarrido){
	case 1:{
		return '*';
		break;
	}
	case 2:{
		return 0;
		break;
	}
	case 3:{
		return '#';
		break;
	}
//	case 4:{
//		return 'D';
//		break;
//	}
	default:{
		return 0;
		break;
	}
	}
}

void limpiar_casilla(void){
	for(uint8_t pos = 0; pos < 2; pos++){
		casilla[pos] = 0;
	}
}



