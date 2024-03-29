/*
 * PwmDriver.c
 *
 *  Created on: May 9, 2023
 *      Author: isabel
 */


#include "PwmDriver.h"
#include "BasicTimer.h"

/**/
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrPwmHandler->ptrTIMx == TIM2){
		RCC->APB1ENR &= ~ RCC_APB1ENR_TIM2EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		RCC->APB1ENR &= ~ RCC_APB1ENR_TIM3EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM4){
		RCC->APB1ENR &= ~ RCC_APB1ENR_TIM4EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM5){
		RCC->APB1ENR &= ~ RCC_APB1ENR_TIM5EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 1. Cargamos la frecuencia deseada */
	setFrequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	setDuttyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */

	//Configuramos el registro que controla el modo up
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	//Reiniciar el registro del counter
	ptrPwmHandler->ptrTIMx->CNT = 0;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		// Seleccionamos como salida el canal
		//Limpiamos los bits correspondientes
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC1S;

		//Para que sea una salida más rapida
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1FE;

		// Configuramos el canal como PWM
		//Limpiamos esas posiciones de memoria
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC1M;

		//Con un 110 se activa el PWM modo 1
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1M_2;
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1M_1;

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1PE;

		break;
	}

	case PWM_CHANNEL_2:{
		// Seleccionamos como salida el canal
		//Limpiamos los bits correspondientes
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC2S;

		//Para que sea una salida más rápida
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2FE;

		// Configuramos el canal como PWM
		//Limpiamos las posicioens de memoria
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC2M;

		//Con un 110 se activa el PWM modo 1
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2M_2;
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2M_1;

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;

		break;
	}

	case PWM_CHANNEL_3:{
		// Seleccionamos como salida el canal
		//Limpiamos los bits correspondientes
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC3S;

		//Para que sea una salida más rapida
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3FE;

		// Configuramos el canal como PWM
		//Limpiamos esas posiciones de memoria
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC3M;

		//Con un 110 se activa el PWM modo 1
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3M_2;
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3M_1;

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3PE;

		break;
	}

	case PWM_CHANNEL_4:{
		// Seleccionamos como salida el canal
		//Limpiamos los bits correspondientes
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC4S;

		//Para que sea una salida más rapida
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4FE;

		// Configuramos el canal como PWM
		//Limpiamos esas posiciones de memoria
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC4M;

		//Con un 110 se activa el PWM modo 1
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4M_2;
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4M_1;

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4PE;
	}

	default:{
		break;
	}

	}// fin del switch-case
	/* 6. Activamos la salida seleccionada */
//	enableOutput(ptrPwmHandler);
}

/* Función para activar el Timer y activar todo el módulo PWM */
void startPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

}

/* Función para desactivar el Timer y detener todo el módulo PWM*/
void stopPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
	ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1E;
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */

void enableOutput(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1P;
		break;
	}

	case PWM_CHANNEL_2: {
		// Activamos la salida del canal 2
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC2P;
		break;
	}

	case PWM_CHANNEL_3: {
		// Activamos la salida del canal 3
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC3P;
		break;
	}

	case PWM_CHANNEL_4: {
		// Activamos la salida del canal 4
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4P;
		break;
	}

	default: {
		break;
	}
	}
}

/*
 * La frecuencia es definida por el conjunto formado por el preescaler (PSC)
 * y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
void setFrequency(PWM_Handler_t *ptrPwmHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer

	ptrPwmHandler->ptrTIMx->PSC = ptrPwmHandler->config.prescaler;

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	ptrPwmHandler->ptrTIMx->ARR = ptrPwmHandler->config.periodo - 1;

}


/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){
	// Actualizamos el registro que manipula el periodo
    ptrPwmHandler->config.periodo = newFreq;

	// Llamamos a la fucnión que cambia la frecuencia
	setFrequency(ptrPwmHandler);
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void setDuttyCycle(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	case PWM_CHANNEL_2:{
		ptrPwmHandler->ptrTIMx->CCR2 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	case PWM_CHANNEL_3:{
		ptrPwmHandler->ptrTIMx->CCR3 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	case PWM_CHANNEL_4:{
		ptrPwmHandler->ptrTIMx->CCR4 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	default:{
		break;
	}

	}// fin del switch-case

}


/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){
	ptrPwmHandler->config.duttyCicle = newDutty;

	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
	setDuttyCycle(ptrPwmHandler);
}


//Función encargada de desactivar cada uno de los canales con los que cuenta el TimerX
void disableOutput(PWM_Handler_t *ptrPwmHandler){
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Desactivamos la salida del canal 1
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1E;
		break;
	}

	case PWM_CHANNEL_2: {
		// Desactivamos la salida del canal 2
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC2E;
		break;
	}

	case PWM_CHANNEL_3: {
		// Desactivamos la salida del canal 3
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC3E;
		break;
	}

	case PWM_CHANNEL_4: {
		// Desactivamos la salida del canal 4
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4E;
		break;
	}

	default: {
		break;
	}
	}


}
