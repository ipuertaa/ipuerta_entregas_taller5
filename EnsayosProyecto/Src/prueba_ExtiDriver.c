/*
 * prueba_ExtiDriver.c
 *
 *  Created on: Mar 25, 2023
 *      Author: isabel
 */


#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


GPIO_Handler_t handlerLED2 = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
GPIO_Handler_t handlerUserButton = {0};
EXTI_Config_t handlerExti = {0};

uint32_t counterExti13 = 0;




//Cabeceras de funciones

void init_hardware(void);
void EXTI15_10_IRQHandler(void);
void BasicTimer2_Callback(void);



int main(void){

	init_hardware();
	while(1){

	}

}	//Fin main

void init_hardware(void){

	//configurar LED2 - PA5

	handlerLED2.pGPIOx =GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//cargar la configuración del LED a los registros del MCU
	GPIO_Config(&handlerLED2);

	//Configurar el TIM2 para que haga un blinky cada 300 ms

	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 300;  	//lanza una interrupción cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	//Cargar la configuración del TIM2 en los registros
	BasicTimer_Config(&handlerBlinkyTimer);

	//Configuración del user button

	handlerUserButton.pGPIOx										= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber	= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;


	//Configuración el exti
	handlerExti.pGPIOHandler = &handlerUserButton;		//Debo cargar la posición de memoria
	handlerExti.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti
	extInt_Config(&handlerExti);

}

void callback_extInt13(void){
	counterExti13++;
}


void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}
