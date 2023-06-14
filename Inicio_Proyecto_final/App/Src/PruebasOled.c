/*
 * PruebasOled.c
 *
 *  Created on: Jun 14, 2023
 *      Author: isabel
 */


#include <stm32f4xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "OledDriver.h"
#include "I2CDriver.h"
#include "SysTickDriver.h"

#define oLEDAddress 			0x3C


// Elementos para el uso del acelerómetro:
GPIO_Handler_t handlerOledSDA = {0};
GPIO_Handler_t handlerOledSCL = {0};
I2C_Handler_t handlerOLED = {0};



void init_hardware(void);

int main(void){


	init_hardware();
	initOled(&handlerOLED);

	OLED_print_msg(&handlerOLED, "T U V W X Y Z 0 1 2 3 4 5 6 7 8 9 : = $");
	while(1){

	}
	return 0;

}


void init_hardware(void){

	// Configurar los elementos para el manejo de la pantalla oled

	handlerOledSCL.pGPIOx									= GPIOB;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinOPType   			= GPIO_OTYPE_OPENDRAIN;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_PULLUP;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerOledSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerOledSCL);

	handlerOledSDA.pGPIOx 								= GPIOB;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinOPType   		= GPIO_OTYPE_OPENDRAIN;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLUP;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerOledSDA.GPIO_PinConfig.GPIO_PinAltFunMode	= AF4;

	GPIO_Config(&handlerOledSDA);


	handlerOLED.ptrI2Cx				= I2C1;
	handlerOLED.modeI2C				= I2C_MODE_FM;
	handlerOLED.slaveAddress		= oLEDAddress;
	handlerOLED.Clock_Freq			= CLOCK_FREQ_16MHz;

	i2c_config(&handlerOLED);

	config_SysTick_ms(HSI_CLOCK_CONFIGURED);
}



