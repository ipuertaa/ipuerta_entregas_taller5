/*
 * ejercicioEXTI.c
 *
 *  Created on: Mar 24, 2023
 *      Author: isabel
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/* Siempre configurar el led de estado para mostrar que el sistema está funcionando bien*/


//Definición de los elementos del sistema

GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
uint32_t counterExti13 = 0;

//Cabeceras de funciones
void init_hardware(void);
void callback_exti13(void);

int main(void){

	//inicializamos todos los elementos del sistema con la función init_hardware
	init_hardware();
	while(1){

	}

}	 //Fin main






//DEFINICIÓN DE FUNCIONES

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

	//GPIO_WritePin(&handlerLED2, SET);


	//Configurar el TIM2 para que haga un blinky cada 300 ms

	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 300;  	//lanza una interrupción cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	//Cargar la configuración del TIM2 en los registros
	BasicTimer_Config(&handlerBlinkyTimer);


	//Configuración del EXTI - user button

	//1. Configurar pin solo entrada digital
	handlerUserButton.pGPIOx =GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode =GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerUserButton);

	//2. Activar la señal de reloj del SYSCFG

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//3. Configurar el multiplexor 13 para que utilice el puerto c 0xF --> 0b1111

	SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	//4. Configuración del EXTI

	//4.a Configurar el tipo de flanco, subida o bajada.

	EXTI->FTSR = 0; 		//Descativamos todos los posibles flancos de bajada
	EXTI->RTSR = 0;			//Llevando el registro a un valor conocido
	EXTI->RTSR |= EXTI_RTSR_TR13;

	//4.b Activar la interrupción con la máscara

	EXTI->IMR = 0;
	EXTI->IMR |= EXTI_IMR_IM13;		//Activamos la interrupción 13

	//5 Configurar el NVIC

	//5.a Desactivar todas las interrupciones
	__disable_irq();

	//5.b matricular la interrupción en el NVIC
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	//5.c Crear ISR
	//5.d Crear callback
	//5.e Activar interrupciones
	__enable_irq();


} //fin init_Hardware

//
void callback_exti13(void){
	counterExti13++;
}


void EXTI15_10_IRQHandler(void){

	//Verificar la interrupción que se lanzó
	if((EXTI->PR & EXTI_PR_PR13) != 0){
		EXTI->PR |= EXTI_PR_PR13; 			//Limpiar la bandera.
		callback_exti13();
	}

}
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}
