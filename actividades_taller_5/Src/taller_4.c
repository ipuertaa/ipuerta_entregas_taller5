/*
 * taller_4.c
 *
 *  Created on: Mar 2, 2023
 *      Author: isabel
 */

//Con esta librería es que puedo mejorar la notación de las variables
#include <stdint.h>

//definir las variables

uint8_t variable_1 = 0b1011001;
uint8_t variableResultado;
uint8_t variableMultiplicacion = 5;

//en la variable_resultado sólo voy a guardar el valor
//Quiero volver cero la cuarta posicion
int main (void){

	variableResultado = variable_1 & ~(1<<3);

// Quiro que se vea reflejado el número nuevo en variable_resultado
	variable_1 = variable_1 & ~(1<<3);

//Que se actualice la misma variable con el cambio hecho

//	variable_1 += 5;
//Con esto puedo sumar 5 a la variable_1. Sirve para simplificar la notación y no tener que escribir
//variable_1 dos veces

	variable_1 &= ~(1<<3);
//A variable_1 le estoy haciendo un and con el shift y guardándolo en variable_1

//Los break points sirven para saltarse partes del código
//Con resume corro todo el código o hasta un break point
	variableMultiplicacion <<= 3;
//El 3 indica cuántas veces multiplico, siempre es x2
//Con shift a la izquierda, multiplico por 2 n veces

	variableMultiplicacion >>= 2;
//Con shift a la derecha divido entre 2 x número de veces
	}


