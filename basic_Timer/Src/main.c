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

#include <stdint.h>
#include <stm32f4xx.h>

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


	/* CONFIGURACIÓN DEL TIMER */

	//ACTIVANDO LA SEÑAL DE RELOJ

	RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
	RCC->AHB1LPENR |= RCC_APB1ENR_TIM2EN;

	//DIRECCIÓN

	TIM2->CR1 &= ~TIM_CR1_DIR;

	//PRESCALER

	TIM2->PSC = 16000;

	//CONFIGURAR EL COUNTER

	TIM2->CNT = 0;

	//CONFIGURAR EL ARR

	TIM2->ARR = 250;

	//ACTIVAR EL TIMER

	TIM2->CR1 |= TIM_CR1_CEN;

	while(1){
		//cuando se actualiza el counter --> bandera. Después de que se active, yo debo desactivarlo

//		if (TIM2->SR & TIM_SR_UIF){
//			TIM2->SR &= ~TIM->SR_UIF;
//

		}


	}



