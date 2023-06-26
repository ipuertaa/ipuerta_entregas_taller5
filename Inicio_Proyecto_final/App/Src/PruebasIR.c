/*
 * PruebasIR.c
 *
 *  Created on: Jun 26, 2023
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
#include "PwmDriver.h"


GPIO_Handler_t handlerLED2 					= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};

GPIO_Handler_t handlerEstadoC1 = {0};
EXTI_Config_t handlerEXTIC1 = {0};
uint8_t auxEXTI = 0;
uint8_t producto = 0;

void init_hardware(void);

int main(void){

	init_hardware();

	while(1){

		if(auxEXTI == 1){
			producto = 0;
			auxEXTI = 0;
		}

	}
}

void init_hardware(void){

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


	handlerEstadoC1.pGPIOx 									= GPIOC;
	handlerEstadoC1.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerEstadoC1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerEstadoC1.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerEstadoC1.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	//Configuración del EXTI

	handlerEXTIC1.pGPIOHandler = &handlerEstadoC1;
	handlerEXTIC1.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;

	//Cargar la configuración del exti y GPIO para la F1
	extInt_Config(&handlerEXTIC1);

}

void callback_extInt5(void){
	auxEXTI = 1;
	producto = 1;
}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}


