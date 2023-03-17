/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez
 * @brief          : Main program body
 ******************************************************************************
 * Solución de la tarea #2 de taller 5.
 * En este archivo se escribe todo el código a ejecutar para realizar los
 * puntos de la tarea.
 ******************************************************************************
 */


/* PRIMER PUNTO:
 *
 * El error corresponde a que, en la función GPIO_ReadPin() sólo se está trasladando hacia la posicion menos
 * significativa del registro IDR el bit particular que se quiere leer.
 *
 * Sólo se está moviendo, pero lo que haya a la izquiera de ese bit, sigue estando, y al retornar PinValue no
 * se tendrá únicamente el dato de 1 bit, sino otros que no me interesan (Cuando quiero leer un pin,
 * sólo necesito ese pin, no el resto)
 *
 * Este error se soluciona con una máscara con una operación AND, poniendo un 1 en la posición menos
 * significativa y el resto con ceros. Con esto estamos limpiando la información que no necesito y dejando sólo
 * lo que sí necesito.
 *
 * Esto se hace añadiendo la siguiente línea de código en el GPIOxDRIVER.c
 *
 * pinValue &= (1<<0);
 *
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"


//Esta es una variable se crea para comprobar el punto 1.

uint32_t ButtonValue;


int main(void){

	// PARA PROBAR EL PUNTO 1

	//Definimos el handler para el PIN que deseamos configurar.
	GPIO_Handler_t handlerUserButton = {0};

	//Según el user manual, el botón "B1 USER" corresponde al puerto C (GPIOC) y al pin 13.
	//Además, al tratarse de un botón, debe ser configurado como entrada.
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	//cargamos la configuración al PIN específico.
	//Esta función va al GPIOxDriver.c y configura los parámetros de la parte de arriba (como el #pin, el modo...)
	//en cada registro correspondiente.
	GPIO_Config(&handlerUserButton);

	//Leemos el pin. Esta función también se remite al GPIOxDriver.c, realiza el proceso correspondiente a la
	//lectura del pin y en ella se guarda el valor del bit correspondiente al pin 13 en el IDR.
	GPIO_ReadPin(&handlerUserButton);


	//Ciclo principal.

	while(1){ 					//Se utiliza un break point para correr el código más rápidamente.

//En la variable ButtonValue se está guardando el lo que se lee en el registro IDR mediante la función GPIO_ReadPin
//oprimido cada vez que se oprime o no el user button. Con el debugger se aprecia que, mientras el botón no
//esté siendo se lee un valor de 1 y cuando se oprime, el valor va a cero.

		ButtonValue = GPIO_ReadPin(&handlerUserButton);

	}
}



