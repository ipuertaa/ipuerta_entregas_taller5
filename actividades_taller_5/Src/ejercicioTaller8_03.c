/*
 * ejercicioTaller8_03.c
 *
 *  Created on: Mar 8, 2023
 *      Author: isabel
 */


//importar las librerías

#include <stdint.h> //Librería para las variables
#include <stdbool.h> //librería para variables booleanas
#include <math.h>	//librería para funciones matemáticas

#define NOP() __asm("NOP")  //Lo que está con # son directivas de pre-procesamiento. No terminan en ;
#define CONSTANTE 100
#define UNSIGNED 0
#define SIGNED 1

uint64_t getMaxBits(uint8_t nBits, uint8_t signo){

	if (signo == UNSIGNED){
		return (pow(2,nBits)-1);
	}
	else{
		return ((pow(2,nBits)/2)-1);
	}

}


int main(void){
//debo definir una variable para poder llamar la función y que el resultado se guarde ahí

}


//Definicion de variables
