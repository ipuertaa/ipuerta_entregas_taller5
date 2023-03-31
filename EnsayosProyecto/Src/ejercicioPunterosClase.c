/*
 * ejercicioPunterosClase.c
 *
 *  Created on: Mar 24, 2023
 *      Author: isabel
 */


#include "stdint.h"
#include "stm32f4xx.h"

/* Definición de elementos del programa */

uint32_t counterG2 = 0;
uint32_t auxVariableG2 = 0;

uint32_t *ptr_CounterG2;

uint32_t byteVariableG2;
uint8_t *ptr_ByteVariableG2;

int main(void){

	//trabajando con las variables y los punteros

	counterG2 = 3456789;  //Asignar un valor a la variable
	auxVariableG2 = counterG2;	//C es pasador por valor. Se le asigna el valor de esa variable

	//al puntero como variable le asigno una posición de memoria
	ptr_CounterG2 = &counterG2;

	//con * puedo modificar esa posición de memoria guardada en el puntero
	*ptr_CounterG2 = 9876543;	//escribirmos un dato en la casilla de memoria donde el puntero apunta


	ptr_CounterG2++;	//movemos una posición de memoria el puntero
	*ptr_CounterG2 = 9876543;	//escribo en la nueva posición de memoria

// ptr_variable = (unit32_t *)auxvariable.   casteo hacia un puntero.



	while(1){


	}
}	//Fin main
