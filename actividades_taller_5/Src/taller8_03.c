/*
 * taller8_03.c
 *
 *  Created on: Mar 8, 2023
 *      Author: isabel
 *
 *  En este taller se tratan los temas de funciones y programación en C
 *
 */

//importar las librerías

#include <stdint.h> //Librería para las variables
#include <stdbool.h> //librería para variables booleanas
#include <math.h>	//librería para funciones matemáticas

#define NOP() __asm("NOP")  //Lo que está con # son directivas de pre-procesamiento. No terminan en ;
#define CONSTANTE 100

// Definicion de prototipos de variables- para cuando defino variables debajo del main
void miPrimerFuncion(void);
uint8_t getMaxChar(void);
uint16_t getMaxValue(uint16_t x, uint16_t y, uint16_t z);

//definir variables con la librería tipo booleano. Variables de 8 bits que solo toman 1 o cero (true o false)

bool miPrimeraVariableGlobal = true;


int main(void){
	/*miPrimerFuncion();  //como es tipo void no pongo nada adentro. Aquí se llama la funcion

	uint8_t maxChar = getMaxChar();
	(void) maxChar;   //esta linea se pone para evitar el warning*/

	uint16_t maxValue = getMaxValue(150, 0b00101101, 0xFEA);
	(void) maxValue;

}

//Si la variable se define por fuera del main se llaman globales. Y existen dentro de todo el codigo

void miPrimerFuncion(void){     //Siempre que se usa esta funcion es porque se va a hacer algo muy especifico

	miPrimeraVariableGlobal = false;

}

uint8_t getMaxChar(void){
	uint8_t charBits = 8;
	uint8_t maxChar = (2*2*2*2*2*2*2*2)-1;
	maxChar = pow(2,charBits)-1;         //Esto es equivalente a lo de arriba con la libretría
	(void) charBits;
	return maxChar;
}

uint16_t getMaxValue(uint16_t x, uint16_t y, uint16_t z){  //Aquí solo la defino, en el main se ejecuta
	if ((x>=y) && (x>=z)){
		return x;
	}
	else if((y>=x) && (y>=z)){
		return y;
	}
	else if ((z>=x) && (z>=y)){
		return z;
	}
	else {
		return 0;
	}
}
