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
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
GPIO_Handler_t handlerLED2 					= {0};



void init_hardware(void);

int main(void){


	init_hardware();
	initOled(&handlerOLED);

//	OLED_print_msg(&handlerOLED, "T U V W X Y Z");
//	OLED_print_msg_pag(&handlerOLED, 2, "HOLA");
	OLED_print_msg(&handlerOLED, "FUNCIONA POR FAVOR JIJIJ");
	while(1){

	}
	return 0;

}


void init_hardware(void){

	handlerLED2.pGPIOx 										= GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLED2);

	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);

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

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}



