/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez
 * Created on: 03/03/23
 ******************************************************************************

 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


//Def el elemento

GPIO_Handler_t handlerTransistor1 = {0};
GPIO_Handler_t handlerTransistor2 = {0};
GPIO_Handler_t handlerLedA = {0};
GPIO_Handler_t handlerLedB = {0};
GPIO_Handler_t handlerLedC = {0};
GPIO_Handler_t handlerLedD = {0};
GPIO_Handler_t handlerLedE = {0};
GPIO_Handler_t handlerLedF = {0};
GPIO_Handler_t handlerLedG = {0};
GPIO_Handler_t handlerLED2 = {0};

GPIO_Handler_t handlerButton = {0};
GPIO_Handler_t handlerEncoder = {0};
GPIO_Handler_t handlerSeñal = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};
BasicTimer_Handler_t handlerTransistores = {0};


EXTI_Config_t handlerExtiEncoder = {0};
EXTI_Config_t handlerExtiButton = {0};


uint8_t auxBoton = 0;
uint8_t auxEncoder = 0;



//Cabeceras de funciones
void init_hardware(void);

int main(void){

	init_hardware();

	while(1){


	}

} //Fin main



void init_hardware(void){

	//configurar LED2 - PA5 - LED DE ESTADO

	handlerLED2.pGPIOx =GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//cargar la configuración del LED a los registros del MCU
	GPIO_Config(&handlerLED2);


	//Configuración del timer para hacer el parpadeo del led de estado

	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 300;  	//lanza una interrupción cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	//Cargar la configuración del TIM2 en los registros
	BasicTimer_Config(&handlerBlinkyTimer);


	//Configuración del pin que controla el transistor 1 -> segmento derecho

	handlerTransistor1.pGPIOx						= GPIOC;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;


	GPIO_Config(&handlerTransistor1);
	GPIO_WritePin(&handlerTransistor1, SET);



	//Configuración del pin que controla el transistor 2 -> segmento izquierdo

	handlerTransistor2.pGPIOx						= GPIOC;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;


	GPIO_Config(&handlerTransistor2);
	GPIO_WritePin(&handlerTransistor2, RESET);


	//Configuración del la posición A
	handlerLedA.pGPIOx						= GPIOC;
	handlerLedA.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerLedA.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedA);
	GPIO_WritePin(&handlerLedA, RESET);


	//Configuración del la posición B
	handlerLedB.pGPIOx						= GPIOA;
	handlerLedB.GPIO_PinConfig.GPIO_PinNumber	= PIN_12;
	handlerLedB.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedB.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedB.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedB);
	GPIO_WritePin(&handlerLedB, RESET);


	//Configuración del la posición C
	handlerLedC.pGPIOx						= GPIOA;
	handlerLedC.GPIO_PinConfig.GPIO_PinNumber	= PIN_11;
	handlerLedC.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedC.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedC.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedC);
	GPIO_WritePin(&handlerLedC, RESET);


	//Configuración del la posición D
	handlerLedD.pGPIOx						= GPIOB;
	handlerLedD.GPIO_PinConfig.GPIO_PinNumber	= PIN_12;
	handlerLedD.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedD.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedD.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedD);
	GPIO_WritePin(&handlerLedD, RESET);


	//Configuración del la posición E
	handlerLedE.pGPIOx						= GPIOB;
	handlerLedE.GPIO_PinConfig.GPIO_PinNumber	= PIN_2;
	handlerLedE.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedE.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedE.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedE.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedE);
	GPIO_WritePin(&handlerLedE, RESET);


	//Configuración del la posición F
	handlerLedF.pGPIOx						= GPIOB;
	handlerLedF.GPIO_PinConfig.GPIO_PinNumber	= PIN_1;
	handlerLedF.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedF.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedF.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedF.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedF);
	GPIO_WritePin(&handlerLedF, RESET);


	//Configuración del la posición G
	handlerLedG.pGPIOx						= GPIOB;
	handlerLedG.GPIO_PinConfig.GPIO_PinNumber	= PIN_15;
	handlerLedG.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedG.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedG.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedG.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedG);
	GPIO_WritePin(&handlerLedG, RESET);


	// Configuración del timer para controlar los transistores

	handlerTransistores.ptrTIMx = TIM3;
	handlerTransistores.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTransistores.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerTransistores.TIMx_Config.TIMx_period = 3;
	handlerTransistores.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	//Cargar la configuración del TIM3 en los registros
	BasicTimer_Config(&handlerTransistores);


	/* 		CONFIGURACIÓN DE LAS INTERRUPCIONES			*/

	//Configuración del GPIO del botón

	handlerButton.pGPIOx =GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerButton.GPIO_PinConfig.GPIO_PinMode =GPIO_MODE_IN;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Configuración del EXTI del botón

	handlerExtiButton.pGPIOHandler = &handlerButton;
	handlerExtiButton.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;

	extInt_Config(&handlerExtiButton);


	//Configuración del GPIO del encoder

	handlerEncoder.pGPIOx =GPIOB;
	handlerEncoder.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	handlerEncoder.GPIO_PinConfig.GPIO_PinMode =GPIO_MODE_IN;
	handlerEncoder.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Configuración del EXTI del encoder

	handlerExtiEncoder.pGPIOHandler = &handlerEncoder;
	handlerExtiEncoder.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	extInt_Config(&handlerExtiEncoder);



}	//Fin init_hardware

void callback_extInt9(void){		//Callback del botón
	auxBoton++;

}

void callback_extInt8(void){		//Callback del encoder
	auxEncoder++;

}



void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}

void BasicTimer3_Callback(void){
	GPIO_TooglePin(&handlerTransistor2);
	GPIO_TooglePin(&handlerTransistor1);
}



