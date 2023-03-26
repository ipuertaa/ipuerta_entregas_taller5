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
#include "BasicTimer.h"


//Def el elemento

GPIO_Handler_t handlerLed2 = {0};
BasicTimer_Handler_t handlerTimer5 = {0};

uint8_t flag = 0;

int main(void){

	handlerTimer5.ptrTIMx 							= TIM5;
	handlerTimer5.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimer5.TIMx_Config.TIMx_speed			= BTIMER_SPEED_1ms;
	handlerTimer5.TIMx_Config.TIMx_period			= 250;
	handlerTimer5.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerTimer5);

	handlerLed2.pGPIOx							= GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLed2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLed2);

//	GPIO_WritePin(&handlerLed2, SET);
	while(1){
	}

} //Fin main
void BasicTimer5_Callback(void){
	GPIO_TooglePin(&handlerLed2);
}
