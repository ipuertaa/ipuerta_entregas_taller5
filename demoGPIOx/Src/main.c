/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez
 * Created on: 03/03/23
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"  //incluyo el código del video

int main(void){

	// Configuracion inicial del MCU. Aquí voy a hacer los pasos del tablero

//	RCC->AHB1ENR = 0x0001;  //haciéndolo con igualdad, podemos descuadrar lo de algún otro periférico
	//Hacemos una máscara que sólo modifique el bit que necesito. Antes de hacer la máscara debo limpiar la posición.

	RCC->AHB1ENR &= ~(1<<0);	//Aquí se limpia la posición cero. SIEMPRE SE LIMPIA ANTES DE CONFIGURAR
	RCC->AHB1ENR |= (1<<0);	//Activamos la señal de reloj

	//Configurar el pin PA5 como salida
	GPIOA->MODER &= ~(0b11<<10);  //Limpiando las posiciones 11 y 10 del MODER
	GPIOA->MODER |= (0b01<<10);  //configurando el pinA5 como salida

	//Configurando el OTYPER

	GPIOA->OTYPER &= ~(1<<5);		//Limpiando la posición. Ya queda como push-pull

	//Configurando el OSPEED
	GPIOA->OSPEEDR &= ~(0b11<<10);	//Limpiando las posiciones 10 y 11
	GPIOA->OSPEEDR |= (0b10<<10);	//Configurando velocidad de salida en Fast

	//Configuraciones de las resistencias de PU-PD

	GPIOA->PUPDR &= ~(0b11<<10);    //Limpiando las posiciones 11 y 10 para que sea no-PUPD


	GPIOA->ODR &= ~(1<<5);		//Limpiando la salida del PA5. Apaga el LED
	GPIOA->ODR |= (1<<5);		//Se enciende el LED

	while(1){

	}
}
