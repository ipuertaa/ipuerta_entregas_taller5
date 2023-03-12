/*
 * GPIOxDriverMain.c
 *
 *  Created on: Mar 11, 2023
 *      Author: isabel
 *
 *      Con este programa se muestra el uso básico de los registros que controlan al micro (SFR) y la forma
 *      adecuada de utilizar los operadores &, |, ~, =, para cambiar la configuración de algún registro.
 *      También es importante entender la utilidad de los números BIN y HEX
 *
 *      Es necesario tener a la mano el manual de referencia y la hoja de datos del micro.
 *
 *      HAL: Hardware Abstraction Layer
 *
 *      Este programa introduce el periférico más simple que tiene el micro, que es el encargado de manejar
 *      los pones de cada puerto del micro.
 *
 *      Cada PINx de cada puerto GPIO puede desarrollar funciones básicas de tipo de entrada y salida de datos
 *      digitales, además se les puede asignar funciones especiales que generalmente están ligadas a otro periférico
 *      adicional.
 *
 *      Varias características pueden ser configuradas para cada PIN:
 *      - velocidad de respuesta
 *      - tipo de entrada (pull-up, pull-down, open drain (flotante))
 *      - tipo de salida (push-pull, open drain...)
 *      - entrada análoga
 *
 *      Debemos definir todos los registros que manejan el periférico GPIOx y luego crear algunas funciones
 *      para utilizar adecuadamente el registro
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"


//Función principal del programa, donde se ejecuta todo

int main(void){

	//Definimos el handler para el PIN que deseamos configurar
	GPIO_Handler_t handlerUserLedPin = {0}

	//deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0				//Ninguna función


	//cargamos la configuración al PIN específico
	GPIO_Config(&handlerUserLedPin);

	//hacemos que el PIN A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);

	//este es el ciclo principal, donde se ejecuta todo el programa
	while(1){
		NOP();
	}
}


