/*
 * blinkyng.c
 *
 *  Created on: Mar 14, 2023
 *      Author: isabel
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"

#include "GPIOxDriver.h"

//inicializamos el pin con un handler

GPIO_Handler_t handlerOnBoardLed = {0};

//configuramos el Pin


int main(void){

	handlerOnBoardLed.pGPIOx = GPIOA;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //ESTE ES SÓLO PARA ENTRADAS
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&handlerOnBoardLed);


	while(1){
		for(uint32_t contador = 0; contador <= 1000000; contador ++){

		NOP();
		}
		GPIOxTooglePin(&handlerOnBoardLed);

	}


}
