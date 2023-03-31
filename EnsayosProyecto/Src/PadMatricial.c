/*
 * PadMatricial.c
 *
 *  Created on: Mar 27, 2023
 *      Author: isabel
 */


#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/* 3 pines de propósito general configurados como columnas serán salidas a un nivel alto.
 * 4 filas serán configuradas como entradas y serán las que causarán la interrupción.
 *
 */

//Definición de variables

uint8_t banderaBarrido = 0;
uint8_t banderaCondicion = 0;

//Definición de los handler

//Handlers del GPIO: Todos los pines, independientes de entrada o salida, se deben configurar

GPIO_Handler_t handlerColumna1 = {0};		//1,4,7,*
GPIO_Handler_t handlerColumna2 = {0};		//2,5,8,0
GPIO_Handler_t handlerColumna3 = {0};		//3,6,9,#

GPIO_Handler_t handlerFila1 = {0};			//1,2,3
GPIO_Handler_t handlerFila2 = {0};			//4,5,6
GPIO_Handler_t handlerFila3 = {0};			//7,8,9
GPIO_Handler_t handlerFila4 = {0};			//*,0,#

//Handler para el timer: con esto podré controlar el barrido de las columnas

BasicTimer_Handler_t handlerBarrido = {0};

//Handlers para los EXTI. Cada fila es una entrada por donde podrá haber una interrupción o no

EXTI_Config_t handlerEXTI1 = {0};		//exti para la fila 1
EXTI_Config_t handlerEXTI2 = {0};		//exti para la fila 2
EXTI_Config_t handlerEXTI3 = {0};		//exti para la fila 3
EXTI_Config_t handlerEXTI4 = {0};		//exti para la fila 4


// Cabeceras de funciones

void init_hardware(void);		//Función para la configuración de pines.

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

	}	//Fin while
}



//		DEFINICIÓN DE LAS FUNCIONES

void init_hardware(void){

	//Configurar la C1 -> PC8

	handlerColumna1.pGPIOx 								= GPIOC;
	handlerColumna1.GPIO_PinConfig.GPIO_PinNumber 		= PIN_8;
	handlerColumna1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna1.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna1.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna1);		//Cargar la configuración


	//Configurar la C2 -> PC6

	handlerColumna2.pGPIOx 								= GPIOC;
	handlerColumna2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
	handlerColumna2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna2);		//Cargar la configuración


	//Configurar la C3 -> PC5

	handlerColumna3.pGPIOx 								= GPIOC;
	handlerColumna3.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
	handlerColumna3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna3.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna3.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna3);		//Cargar la configuración


	//Configurar el TIM3 para hacer el barrido

	handlerBarrido.ptrTIMx 							= TIM3;
	handlerBarrido.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerBarrido.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerBarrido.TIMx_Config.TIMx_period 			= 5000;
	handlerBarrido.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBarrido);

/*  CONFIGURACIÓN DE LAS FILAS   */

	//Configuración del pin para la F1 ->PB1

	handlerFila1.pGPIOx 								= GPIOB;
	handlerFila1.GPIO_PinConfig.GPIO_PinNumber 			= PIN_1;
	handlerFila1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila1.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI1.pGPIOHandler = &handlerFila1;
	handlerEXTI1.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F1
	extInt_Config(&handlerEXTI1);


	//Configuración del pin para la F2 ->PB15

	handlerFila2.pGPIOx 								= GPIOB;
	handlerFila2.GPIO_PinConfig.GPIO_PinNumber 			= PIN_15;
	handlerFila2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila2.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI2.pGPIOHandler = &handlerFila2;
	handlerEXTI2.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F2
	extInt_Config(&handlerEXTI2);



	//Configuración del pin para la F3 ->PB14

	handlerFila3.pGPIOx 								= GPIOB;
	handlerFila3.GPIO_PinConfig.GPIO_PinNumber 			= PIN_14;
	handlerFila3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila3.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI3.pGPIOHandler = &handlerFila3;
	handlerEXTI3.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F3
	extInt_Config(&handlerEXTI3);


	//Configuración del pin para la F4 ->PB13

	handlerFila4.pGPIOx 								= GPIOB;
	handlerFila4.GPIO_PinConfig.GPIO_PinNumber 			= PIN_13;
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


