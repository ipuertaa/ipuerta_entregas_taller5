/*
 * pruebasTeclado.c
 *
 *  Created on: Jun 23, 2023
 *      Author: isabel
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


uint8_t banderaBarrido = 0;
uint8_t banderaCondicion = 0;
char flagFilas  = 0;


uint8_t estado = 0;



//Definición de los handler

//Handlers del GPIO: Todos los pines, independientes de entrada o salida, se deben configurar

GPIO_Handler_t handlerColumna1 = {0};		//1,4,7,*
GPIO_Handler_t handlerColumna2 = {0};		//2,5,8,0
GPIO_Handler_t handlerColumna3 = {0};		//3,6,9,#

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
char identificarF1(void);
char identificarF2(void);
char identificarF3(void);
char identificarF4(void);

int main(void){

	init_hardware();

	while(1){

		if(banderaCondicion == 0){
			switch(banderaBarrido){
			case 1:{
				GPIO_WritePin(&handlerColumna1, SET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, RESET);
				banderaCondicion = 1;
				break;
			}
			case 2: {
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, SET);
				GPIO_WritePin(&handlerColumna3, RESET);
				banderaCondicion = 1;
				break;
			}
			case 3:{
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, SET);
				banderaBarrido = 0;
				banderaCondicion = 1;
				break;

			}
			default:{
				__NOP();
			}


			} //Fin switch
		} 	//Fin if banderaCondicion

		else{
			__NOP();
		}

	}
}

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


	//Configurar la C2

	handlerColumna2.pGPIOx 								= GPIOB;
	handlerColumna2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_0;
	handlerColumna2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna2);		//Cargar la configuración


	//Configurar la C3

	handlerColumna3.pGPIOx 								= GPIOC;
	handlerColumna3.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerColumna3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna3.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna3.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna3);		//Cargar la configuración

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

	handlerBarrido.ptrTIMx 							= TIM3;
	handlerBarrido.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerBarrido.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_100us;
	handlerBarrido.TIMx_Config.TIMx_period 			= 50;
	handlerBarrido.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBarrido);

/*  CONFIGURACIÓN DE LAS FILAS   */

	//Configuración del pin para la F1 -

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


	//Configuración del pin para la

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



	//Configuración del pin para la F3

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


	//Configuración del pin para la F4

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


void BasicTimer3_Callback(void){
	banderaBarrido++;
	banderaCondicion = 0;

}

void callback_extInt3(void){
	flagFilas = identificarF1();
}

void callback_extInt4(void){
	flagFilas = identificarF2();
}

void callback_extInt1(void){
	flagFilas = identificarF3();
}

void callback_extInt0(void){
	flagFilas = identificarF4();
}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
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



