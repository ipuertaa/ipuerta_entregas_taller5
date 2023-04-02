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
GPIO_Handler_t handlerClock = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};
BasicTimer_Handler_t handlerTransistores = {0};


EXTI_Config_t handlerExtiEncoder = {0};
EXTI_Config_t handlerExtiButton = {0};


uint8_t auxBoton = 0;
uint8_t auxEncoder = 0;
int64_t auxConteo = 0;
uint8_t auxClock = 0;
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t banderaIncremento = 0;
uint8_t banderaDecremento = 0;


uint8_t banderaEntrada = 0;
uint8_t banderaBarrido = 0;




//Cabeceras de funciones
void init_hardware(void);
void comparacion(void);
void incrementoUnidades(void);
void incrementoDecenas(void);

void mostrarDecenas(void);
void mostrarUnidades(void);



void num0(void);
void num1(void);
void num2(void);
void num3(void);
void num4(void);
void num5(void);
void num6(void);
void num7(void);
void num8(void);
void num9(void);



int main(void){


	init_hardware();



	while(1){

		if (banderaEntrada == 0){

			switch (banderaBarrido){
			case 1: {//Encender decenas
				GPIO_WritePin(&handlerTransistor1, SET);
				GPIO_WritePin(&handlerTransistor2, RESET);

				if (banderaIncremento == 1){
					incrementoDecenas();
				}	//Fin if incremento
				else if (banderaDecremento == 1){
					incrementoDecenas();
				}	//Fin if else decremento
				else {
					__NOP();
				}

				banderaEntrada = 1;		//para que no entre infinitas veces en el while
				break;
			}	//Fin caso 1

			case 2:{	//Encender unidades

				GPIO_WritePin(&handlerTransistor1, RESET);
				GPIO_WritePin(&handlerTransistor2, SET);

				if (banderaIncremento == 1){
					incrementoUnidades();
				}	//Fin if incremento
				else if (banderaDecremento == 1){
					incrementoUnidades();
				} 	//Fin if else decremento
				else {
					__NOP();
				}

				banderaBarrido = 0;		//para que se reinicie el barrio
				banderaEntrada = 1;
				break;

			}		//Fin caso 2
			default:{
				__NOP();
				break;
			}
			}		//Fin switch
		}	//Fin if bandera entrada

		else {
			__NOP();
		}


	}//Fin while

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


	//Configuración del pin que controla el transistor 2 -> segmento izquierdo

	handlerTransistor2.pGPIOx						= GPIOC;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;


	GPIO_Config(&handlerTransistor2);




	//Configuración del la posición A
	handlerLedA.pGPIOx						= GPIOC;
	handlerLedA.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerLedA.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedA);



	//Configuración del la posición B
	handlerLedB.pGPIOx						= GPIOA;
	handlerLedB.GPIO_PinConfig.GPIO_PinNumber	= PIN_12;
	handlerLedB.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedB.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedB.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedB);


	//Configuración del la posición C
	handlerLedC.pGPIOx						= GPIOA;
	handlerLedC.GPIO_PinConfig.GPIO_PinNumber	= PIN_11;
	handlerLedC.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedC.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedC.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedC);



	//Configuración del la posición D
	handlerLedD.pGPIOx						= GPIOB;
	handlerLedD.GPIO_PinConfig.GPIO_PinNumber	= PIN_12;
	handlerLedD.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedD.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedD.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedD);



	//Configuración del la posición E
	handlerLedE.pGPIOx						= GPIOB;
	handlerLedE.GPIO_PinConfig.GPIO_PinNumber	= PIN_2;
	handlerLedE.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedE.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedE.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedE.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedE);



	//Configuración del la posición F
	handlerLedF.pGPIOx						= GPIOB;
	handlerLedF.GPIO_PinConfig.GPIO_PinNumber	= PIN_1;
	handlerLedF.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedF.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedF.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedF.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedF);



	//Configuración del la posición G
	handlerLedG.pGPIOx						= GPIOB;
	handlerLedG.GPIO_PinConfig.GPIO_PinNumber	= PIN_15;
	handlerLedG.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLedG.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedG.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedG.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedG);
	GPIO_WritePin(&handlerLedG, SET);



	// Configuración del timer para controlar los transistores

	handlerTransistores.ptrTIMx = TIM3;
	handlerTransistores.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTransistores.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerTransistores.TIMx_Config.TIMx_period = 10;
	handlerTransistores.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	//Cargar la configuración del TIM3 en los registros
	BasicTimer_Config(&handlerTransistores);


	/* 		CONFIGURACIÓN DE LAS INTERRUPCIONES			*/



	//Configuración del GPIO del encoder

	handlerEncoder.pGPIOx =GPIOB;
	handlerEncoder.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	handlerEncoder.GPIO_PinConfig.GPIO_PinMode =GPIO_MODE_IN;
	handlerEncoder.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Configuración del EXTI del encoder

	handlerExtiEncoder.pGPIOHandler = &handlerEncoder;
	handlerExtiEncoder.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;

	extInt_Config(&handlerExtiEncoder);



	//Configuración del GPIO del botón

	handlerButton.pGPIOx		= GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Configuración del EXTI del boton
	handlerExtiButton.pGPIOHandler = &handlerButton;
	handlerExtiButton.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;

	extInt_Config(&handlerExtiButton);

	//Configuración del GPIO para el reloj

	handlerClock.pGPIOx = GPIOB;
	handlerClock.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerClock.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerClock.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerClock);




}	//Fin init_hardware


void num0(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, SET);
}

void num1(void){
	GPIO_WritePin(&handlerLedA, SET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, SET);
}

void num2(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, SET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num3(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num4(void){
	GPIO_WritePin(&handlerLedA, SET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num5(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, SET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num6(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, SET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num7(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, SET);
}

void num8(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num9(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}


void mostrarDecenas(void){
	switch (decenas){

	case 0: {
		num0();
		break;
	}
	case 1: {
		num1();
		break;
	}
	case 2: {
		num2();
		break;
	}
	case 3: {
		num3();
		break;
	}
	case 4: {
		num4();
		break;
	}
	case 5: {
		num5();
		break;
	}
	case 6: {
		num6();
		break;
	}
	case 7: {
		num7();
		break;
	}
	case 8: {
		num8();
		break;
	}
	case 9: {
		num9();
		break;
	}
	default:{
		__NOP();
		break;
	}
	} 		//Fin switch decenas
}	//Fin mostrar decenas


void mostrarUnidades(void){
	switch (unidades){

	case 0: {
		num0();
		break;
	}
	case 1: {
		num1();
		break;
	}
	case 2: {
		num2();
		break;
	}
	case 3: {
		num3();
		break;
	}
	case 4: {
		num4();
		break;
	}
	case 5: {
		num5();
		break;
	}
	case 6: {
		num6();
		break;
	}
	case 7: {
		num7();
		break;
	}
	case 8: {
		num8();
		break;
	}
	case 9: {
		num9();
		break;
	}
	default:{
		__NOP();
		break;
	}
	} 		//Fin switch decenas
}	//Fin mostrar unidades



void incrementoDecenas(void){

	if (auxConteo <= 0){
		auxConteo = 0;
		decenas = 0;
		mostrarDecenas();
	}
	else if (auxConteo < 10){
		decenas = 0;
		mostrarDecenas();
	}
	else if ((auxConteo == 10) || (auxConteo < 20)){
		decenas = 1;
		mostrarDecenas();
	}

	else if ((auxConteo == 20) || (auxConteo < 30)){
		decenas = 2;
		mostrarDecenas();
	}

	else if ((auxConteo == 30) || (auxConteo < 40)){
		decenas = 3;
		mostrarDecenas();
	}

	else if ((auxConteo == 40) || (auxConteo < 50)){
		decenas = 4;
		mostrarDecenas();
	}

	else if ((auxConteo == 50) || (auxConteo < 60)){
		decenas = 5;
		mostrarDecenas();
	}

	else if ((auxConteo == 60) || (auxConteo < 70)){
		decenas = 6;
		mostrarDecenas();
	}

	else if ((auxConteo == 70) || (auxConteo < 80)){
		decenas = 7;
		mostrarDecenas();
	}

	else if ((auxConteo == 80) || (auxConteo < 90)){
		decenas = 8;
		mostrarDecenas();
	}
	else if ((auxConteo == 90) || (auxConteo < 100)){
		decenas = 9;
		mostrarDecenas();
	}
	else if(auxConteo >= 100){
		auxConteo = 99;
		decenas = 9;
		mostrarDecenas();
	}
	else {
		__NOP();
	}
}	//Fin incrementoDecenas

void incrementoUnidades(void){

	if (auxConteo <= 0){
		unidades = 0;
		mostrarUnidades();
	}

	if ((auxConteo > 0) && (auxConteo < 10)){
		unidades = auxConteo;
		mostrarUnidades();
	}

	else if ((auxConteo == 10) || (auxConteo < 20)){
		unidades = auxConteo - 10;
		mostrarUnidades();
	}

	else if ((auxConteo == 20) || (auxConteo < 30)){
		unidades = auxConteo - 20;
		mostrarUnidades();
	}

	else if ((auxConteo == 30) || (auxConteo < 40)){
		unidades = auxConteo - 30;
		mostrarUnidades();
	}
	else if ((auxConteo == 40) || (auxConteo < 50)){
		unidades = auxConteo - 40;
		mostrarUnidades();
	}
	else if ((auxConteo == 50) || (auxConteo < 60)){
		unidades = auxConteo - 50;
		mostrarUnidades();
	}
	else if ((auxConteo == 60) || (auxConteo < 70)){
		unidades = auxConteo - 60;
		mostrarUnidades();
	}
	else if ((auxConteo == 70) || (auxConteo < 80)){
		unidades = auxConteo - 70;
		mostrarUnidades();
	}
	else if ((auxConteo == 80) || (auxConteo < 90)){
		unidades = auxConteo - 80;
		mostrarUnidades();
	}
	else if ((auxConteo == 90) || (auxConteo < 100)){
		unidades = auxConteo - 90;
		mostrarUnidades();
	}
	else if(auxConteo >= 100){
		auxConteo = 99;
		unidades = 9;
		mostrarUnidades();
	}
	else {
		__NOP();
	}
}	//Fin IncrementoUnidades






void comparacion(void){

	if (auxEncoder > auxClock){
		banderaIncremento = 1;
		banderaDecremento = 0;
		auxConteo++;

	}
	else if (auxEncoder == auxClock){
		banderaDecremento = 1;
		banderaIncremento = 0;
		auxConteo--;

	}
	else{
		__NOP();
	}

	auxEncoder = 0;

}



void callback_extInt8(void){		//Callback del encoder
	auxEncoder = 1;
	auxClock = GPIO_ReadPin(&handlerClock);
	comparacion();

}


void callback_extInt9(void){		//Callback del boton

	auxBoton++;
}


void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}

void BasicTimer3_Callback(void){
	banderaBarrido++;
	banderaEntrada = 0;
}



