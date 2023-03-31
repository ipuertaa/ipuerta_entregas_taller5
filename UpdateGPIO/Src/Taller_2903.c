/*
 * Taller_2903.c
 *
 *  Created on: Mar 29, 2023
 *      Author: isabel
 */


// Taller 8 - Paso por valor y paso por referencia

#include <stdint.h>

// Creacion de una funcion que duplique el valor de un numero

// 1.0 Paso por valor Básico //usa variables

void duplicarNumero(uint8_t numero){

	//quiero duplicar el número localmente
	numero*=2;
}

// 1.1 Paso por referencia. Usa punteros

void duplicarNumeroRef(uint8_t *numero){

	*numero *=2;
	//con el * siempre se accede al valor dentro de la caja
}

// 1.2 Paso por valor reasignando variables.

uint8_t duplicarNumeroReturn(uint8_t numero){
	return(numero*2);



}

// 1.3 Arreglos por Referencia

void abonarDeudas(uint16_t misDeudas[], uint8_t cantidadDeudas){

	//vamos a recorrer el arreglo

	for(uint8_t i = 0; i<cantidadDeudas; i++){
		//quiero que cada elemento divida el valor a la mitad

		misDeudas[i] /= 2;
	}
}



// ***** // SOLUCION EJERCICIO // ***** //

void stringCaseConverter(uint8_t *string){


	//Aquí estoy recibiendo un puntero. Necesito recorrer todo el arreglo moviendo el puntero

	//Crear un contador
	uint8_t i = 0;

	//Preguntar si llegué al final del arreglo.
	while(string[i] != '\0'){


		//Preguntar si es mayúscula Si el valor en la i esima posición

		if(((*(string+i)) > 64) && (*(string+i) < 91)){
			//Aumentar 32

			*(string+i) = *(string+i) + 32;
		}
		else if (((*(string+i)) > 96) && (*(string+i) < 123)){
			//Disminuir 32
			*(string+i) = *(string+i) - 32;
		}
		else{
			*(string+i) = *(string+i);
		}

	}

		i++;
}




int main(void){

	uint8_t n = 0;

	//pasar por valor

	duplicarNumero(n);

	//Pasar por referencia
	duplicarNumeroRef(&n);

	//reasignación paso por valor
	n = duplicarNumeroReturn(n);

	//pasar por referencia el arreglo
	uint16_t deudasMensuales[5] = {15000,25,1000,0,600};

	/* los arreglos son punteros al primer elemento. El nombre en sí ya es un puntero.
	 */
	abonarDeudas(deudasMensuales, 5);


	/* 1.5 EJERCICIO:

	Crear una función llamada stringCaseConverter que no retorne ningún
	valor, y reciba una string.

	Esta función deberá cambiar las minúsculas por mayúsculas y viceversa
	del string original. */

	uint8_t miPrimerString[] = "HoLLa MuNdO";

	stringCaseConverter(miPrimerString);

	n = 0;







}
