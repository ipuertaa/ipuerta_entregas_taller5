/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez
 * Created on: 03/03/23
 ******************************************************************************
 *
 * En este archivo se definen las directivas de pre-procesamiento para manipular
 * los registros para el uso del HAL
 *
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"


GPIO_Handler_t handlerLed2 = {0};

int main(void){
	//Configuración del pin asociado al led LD2: PA5

		handlerOnBoardLed.pGPIOx 								= GPIOA;
		handlerOnBoardLed.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
		handlerOnBoardLed.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
		handlerOnBoardLed.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerOnBoardLed.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
		handlerOnBoardLed.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;


}


