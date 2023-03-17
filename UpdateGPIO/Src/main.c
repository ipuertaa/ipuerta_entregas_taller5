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


//Def el elemento

GPIO_Handler_t handlerLed2 = {0};

int main(void){


	handlerLed2.pGPIOx		= GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLed2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLed2);

	GPIO_WritePin(&handlerLed2, SET);
	while(1){
		GPIO_TooglePin(&handlerLed2);

		for(int i = 0; i< 2000000; i++){
			__NOP();
		}
	}

}
