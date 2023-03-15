/*
 * punto3.c
 *
 *  Created on: Mar 13, 2023
 *      Author: isabel
 */
#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

//Definición de variables para el punto 3
uint8_t bit0_Aux = 0;
uint8_t bit1_Aux = 0;
uint8_t bit2_Aux = 0;
uint8_t bit3_Aux = 0;
uint8_t bit4_Aux = 0;
uint8_t bit5_Aux = 0;
uint8_t bit6_Aux = 0;
uint8_t numeroBinario = 0;

int main(void){

	//Definición del handler para el pin PC9 que corresponde el Bit-6

	GPIO_Handler_t bit6 = {0};

	//deseamos trabajar con el puerto GPIOC
	bit6.pGPIOx = GPIOC;
	bit6.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	bit6.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	bit6.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	bit6.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	bit6.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	bit6.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&bit6);							//cargamos la configuración

//	GPIO_WritePin(&bit6, SET);					//PIN PC9 quede encendido

	//Definición del handler para el pin PC6 que corresponde el Bit-5

	GPIO_Handler_t bit5 = {0};

	//deseamos trabajar con el puerto GPIOC
	bit5.pGPIOx = GPIOC;
	bit5.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	bit5.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	bit5.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	bit5.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	bit5.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	bit5.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&bit5);							//cargamos la configuración

//	GPIO_WritePin(&bit5, SET);					//PIN PC6 quede encendido


	//Definición del handler para el pin PB8 que corresponde el Bit-4

	GPIO_Handler_t bit4 = {0};

	//deseamos trabajar con el puerto GPIOB
	bit4.pGPIOx = GPIOB;
	bit4.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	bit4.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	bit4.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	bit4.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	bit4.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	bit4.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&bit4);							//cargamos la configuración

//	GPIO_WritePin(&bit4, SET);					//PIN PB8 quede encendido


	//Definición del handler para el pin PA6 que corresponde el Bit-3

	GPIO_Handler_t bit3 = {0};

	//deseamos trabajar con el puerto GPIOA
	bit3.pGPIOx = GPIOA;
	bit3.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	bit3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	bit3.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	bit3.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	bit3.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	bit3.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&bit3);							//cargamos la configuración

//	GPIO_WritePin(&bit3, SET);					//PIN PA6 quede encendido


	//Definición del handler para el pin PC7 que corresponde el Bit-2

	GPIO_Handler_t bit2 = {0};

	//deseamos trabajar con el puerto GPIOC
	bit2.pGPIOx = GPIOC;
	bit2.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	bit2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	bit2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	bit2.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	bit2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	bit2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&bit2);							//cargamos la configuración

//	GPIO_WritePin(&bit2, SET);					//PIN PC7 quede encendido


	//Definición del handler para el pin PC8 que corresponde el Bit-1

	GPIO_Handler_t bit1 = {0};

	//deseamos trabajar con el puerto GPIOC
	bit1.pGPIOx = GPIOC;
	bit1.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	bit1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	bit1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	bit1.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	bit1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	bit1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&bit1);							//cargamos la configuración

//	GPIO_WritePin(&bit1, SET);					//PIN PC8 quede encendido


	//Definición del handler para el pin PA7 que corresponde el Bit-0

	GPIO_Handler_t bit0 = {0};

	//deseamos trabajar con el puerto GPIOA
	bit0.pGPIOx = GPIOA;
	bit0.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	bit0.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	bit0.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	bit0.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	bit0.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	bit0.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				//Ninguna función

	GPIO_Config(&bit0);							//cargamos la configuración

//	GPIO_WritePin(&bit0, SET);					//PIN PA7 quede encendido




	numeroBinario = 0b1010101; //numero 85

	bit0_Aux = numeroBinario &(1<<0);
	bit1_Aux = numeroBinario &(1<<1);
	bit2_Aux = numeroBinario &(1<<2);
	bit3_Aux = numeroBinario &(1<<3);
	bit4_Aux = numeroBinario &(1<<4);
	bit5_Aux = numeroBinario &(1<<5);
	bit6_Aux = numeroBinario &(1<<6);


	if ((bit0_Aux == 0)){
		GPIO_WritePin(&bit0, CLEAR);
	}
	else {
		GPIO_WritePin(&bit0, SET);
	}


	if ((bit1_Aux == 0)){
		GPIO_WritePin(&bit1, CLEAR);
	}
	else {
		GPIO_WritePin(&bit1, SET);
	}


	if ((bit2_Aux == 0)){
		GPIO_WritePin(&bit2, CLEAR);
	}
	else {
		GPIO_WritePin(&bit2, SET);
	}


	if ((bit3_Aux == 0)){
		GPIO_WritePin(&bit3, CLEAR);
	}
	else {
		GPIO_WritePin(&bit3, SET);
	}


	if ((bit4_Aux == 0)){
		GPIO_WritePin(&bit4, CLEAR);
	}
	else {
		GPIO_WritePin(&bit4, SET);
	}


	if ((bit5_Aux == 0)){
		GPIO_WritePin(&bit5, CLEAR);
	}
	else {
		GPIO_WritePin(&bit5, SET);
	}


	if ((bit6_Aux == 0)){
		GPIO_WritePin(&bit6, CLEAR);
	}
	else {
		GPIO_WritePin(&bit6, SET);
	}
}
