/*
 * guessWho.c
 *
 *  Created on: Mar 9, 2023
 *      Author: isabel
 */

#include <stdint.h>

//Definición de las variabes: grupo1 a la primera imágen y grupo 2 a la segunda
uint16_t grupo1 = 0b0000011011001110;
uint16_t grupo2 = 0b1101100001000111;
uint16_t carasMasculinas1;
uint16_t carasMasculinas2;
uint16_t carasFemeninasConLentes1;
uint16_t carasFemeninasConLentes2;
uint32_t variableFinal1;
uint32_t variableFinal2;
uint32_t variableFinal32Bits;

int main(void){
	carasMasculinas1 = ~grupo1;
	carasMasculinas2 = ~grupo2;
	carasFemeninasConLentes1 |= (1<<10);
	carasFemeninasConLentes2 = grupo2 &(0b0101000000000000);
	variableFinal1 = grupo1<<16;
	variableFinal2 = grupo2;
	variableFinal32Bits = variableFinal1 | variableFinal2;

}
