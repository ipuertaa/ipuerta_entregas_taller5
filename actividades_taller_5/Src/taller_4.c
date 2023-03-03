/*
 * taller_4.c
 *
 *  Created on: Mar 2, 2023
 *      Author: isabel
 */

#include <stdint.h>

// Forma comun de definir variables. Tener cuidado con los bits de cada una
//Las variables que defina por fuera serán variables globales. No sacan error
char var1=0;
int var2=0;
short var3=0;
long var4=0;

// Con la siguiente forma nombro funciones con ayuda de la librería.
//siempre debo indicar el numero de bits y poner el _t

uint8_t var5=0;
int8_t var6=0;
int16_t var7=0;
int64_t var8 = 0;


//Siempre debe dar un main.

int main (void){
// Si defino variables dentro del main y no las uso va a aparecer warnings
//Para ver los errores, warnings, etc, click en el martillo.
//Antes de debbugear debo tener 0 errores y warnings y conectar el micro
	uint16_t testShift = 0b0101101010010101;
	uint16_t testMask = 0b0000011100110101;
	while(1){
//Para definir un numero binario debo poner 0b-numero
//Los shift a la izquierda duplican cada vez. Los de la derecha dividen
//con ctrl-espacio me apareden las opciones disponibles cuando este escribiendo

		testShift = testShift << 1;
		var8 = testShift;

	}

}

