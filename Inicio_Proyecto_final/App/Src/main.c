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


/* 4 pines de propósito general configurados como columnas serán salidas a un nivel alto.
 * 4 filas serán configuradas como entradas y serán las que causarán la interrupción.
 *
 */

//Definición de variables

uint8_t banderaBarrido = 0;
uint8_t banderaCondicion = 0;
uint8_t flagFilas  = 0;


char boton = 0;



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


// Cabeceras de funciones

void init_hardware(void);		//Función para la configuración de pines.
void identificarF1(void);
void identificarF2(void);
void identificarF3(void);
void identificarF4(void);

int main(void){

	init_hardware();

	while(1){

		if(banderaCondicion == 0){
			switch(banderaBarrido){
			case 1:{
				GPIO_WritePin(&handlerColumna1, SET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, RESET);
				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;
			}
			case 2: {
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, SET);
				GPIO_WritePin(&handlerColumna3, RESET);
				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;
			}
			case 3:{
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, SET);
				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;

			}
			case 4:{
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, RESET);
				GPIO_WritePin(&handlerColumna4, SET);
				banderaCondicion = 1;
				banderaBarrido = 0;
				break;
			}
			default:{
				__NOP();
			}


			} //Fin switch
		} 	//Fin Barrido de columnas

		else{
			__NOP();
		}


	}	//Fin while
}



//		DEFINICIÓN DE LAS FUNCIONES

void init_hardware(void){

	//Configurar la C1 -> PB7

	handlerColumna1.pGPIOx 								= GPIOB;
	handlerColumna1.GPIO_PinConfig.GPIO_PinNumber 		= PIN_7;
	handlerColumna1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna1.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna1.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna1);		//Cargar la configuración


	//Configurar la C2 -> PB0

	handlerColumna2.pGPIOx 								= GPIOB;
	handlerColumna2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_0;
	handlerColumna2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna2);		//Cargar la configuración


	//Configurar la C3 -> PC13

	handlerColumna3.pGPIOx 								= GPIOC;
	handlerColumna3.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerColumna3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna3.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna3.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna3);		//Cargar la configuración

	//Configurar la C4 -> PC2

	handlerColumna4.pGPIOx 								= GPIOC;
	handlerColumna4.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerColumna4.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna4.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna4.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna4.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna4);		//Cargar la configuración


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

	//Configurar el TIM3 para hacer el barrido

	handlerBarrido.ptrTIMx 							= TIM4;
	handlerBarrido.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerBarrido.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerBarrido.TIMx_Config.TIMx_period 			= 5;
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

}


void BasicTimer4_Callback(void){
	banderaBarrido++;
	banderaCondicion = 0;

}

void callback_extInt3(void){	// F1
	flagFilas = 1;
	identificarF1();
}

void callback_extInt4(void){	//F2
	flagFilas = 2;
	identificarF2();
}

void callback_extInt1(void){	//F3
	flagFilas = 3;
	identificarF3();
}

void callback_extInt0(void){	//F4
	flagFilas = 4;
	identificarF4();
}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}

void identificarF1(void){
	switch(banderaBarrido){
	case 1:{
		boton = 1;
		break;
	}
	case 2:{
		boton = 2;
		break;
	}
	case 3:{
		boton = 3;
		break;
	}
	case 4:{
		boton = 'A';
		break;
	}
	default:{
		break;
	}
	}
}

void identificarF2(void){
	switch(banderaBarrido){
	case 1:{
		boton = 4;
		break;
	}
	case 2:{
		boton = 5;
		break;
	}
	case 3:{
		boton = 6;
		break;
	}
	case 4:{
		boton = 'B';
		break;
	}
	default:{
		break;
	}
	}
}

void identificarF3(void){
	switch(banderaBarrido){
	case 1:{
		boton = 7;
		break;
	}
	case 2:{
		boton = 8;
		break;
	}
	case 3:{
		boton = 9;
		break;
	}
	case 4:{
		boton = 'C';
		break;
	}
	default:{
		break;
	}
	}
}

void identificarF4(void){
	switch(banderaBarrido){
	case 1:{
		boton = '*';
		break;
	}
	case 2:{
		boton = 0;
		break;
	}
	case 3:{
		boton = '#';
		break;
	}
	case 4:{
		boton = 'D';
		break;
	}
	default:{
		break;
	}
	}
}

