/*
 * pruebasADC.c
 *
 *  Created on: Jun 2, 2023
 *      Author: isabel
 */

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
#include "AdcDriver.h"


// Handlers
GPIO_Handler_t handlerStateLED = {0};
GPIO_Handler_t handlerRx = {0};
GPIO_Handler_t handlerTx = {0};

BasicTimer_Handler_t handlerTIM2 = {0};
BasicTimer_Handler_t handlerTIM5 = {0};

USART_Handler_t handlerTerminal = {0};
uint8_t ADC_complete = 0;
uint16_t ADC_data = 0;

uint8_t rxData = 0;

void initSystem(void);

ADC_Config_t chanel0 = {0};
char bufferData[128];

int main(void){

	//El ADC solo funciona a 3.3, nunca conectarlo a 5V

	initSystem();
	writeMsg(&handlerTerminal, "HOLA ESTOY FUNCIONANDO");

	while(1){

		if(ADC_complete == 1){

			sprintf(bufferData, "Data canal 1: %u\n", ADC_data);
			writeMsg(&handlerTerminal, bufferData);

			ADC_complete = 0;

		}
	}

}

void initSystem(void){

	//configurar LED2 - PA5 - LED DE ESTADO

	handlerStateLED.pGPIOx 											= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber 					= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerStateLED);

	handlerTIM2.ptrTIMx  								= TIM2;
	handlerTIM2.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerTIM2.TIMx_Config.TIMx_speed 					= BTIMER_SPEED_1ms;
	handlerTIM2.TIMx_Config.TIMx_period 				= 250;
	handlerTIM2.TIMx_Config.TIMx_interruptEnable 		= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerTIM2);

	//Configuración de la comunicación serial USART2

	handlerTx.pGPIOx 									= GPIOA;
	handlerTx.GPIO_PinConfig.GPIO_PinNumber 			= PIN_2;
	handlerTx.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerTx.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerTx.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerTx);

	handlerRx.pGPIOx 									= GPIOA;
	handlerRx.GPIO_PinConfig.GPIO_PinNumber 			= PIN_3;
	handlerRx.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerRx.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerRx.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerRx);

	handlerTerminal.ptrUSARTx 						= USART2;
	handlerTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_19200;
	handlerTerminal.USART_Config.USART_datasize 	= USART_DATASIZE_8BIT;
	handlerTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerTerminal.USART_Config.USART_mode 		= USART_MODE_RXTX;
	handlerTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerTerminal.USART_Config.USART_enableIntRX 	= USART_RX_INTERRUPT_ENABLE;
	handlerTerminal.USART_Config.USART_enableIntTX 	= USART_TX_INTERRUPT_ENABLE;

	USART_Config(&handlerTerminal);

	handlerTIM5.ptrTIMx  								= TIM5;
	handlerTIM5.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerTIM5.TIMx_Config.TIMx_speed 					= BTIMER_SPEED_1ms;
	handlerTIM5.TIMx_Config.TIMx_period 				= 50;
	handlerTIM5.TIMx_Config.TIMx_interruptEnable 		= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerTIM5);

	chanel0.channel = ADC_CHANNEL_0;
	chanel0.dataAlignment = ADC_ALIGNMENT_RIGHT;
	chanel0.samplingPeriod = ADC_SAMPLING_PERIOD_84_CYCLES;
	chanel0.resolution = ADC_RESOLUTION_12_BIT;

	adc_Config(&chanel0);


}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerStateLED);
}

void BasicTimer5_Callback(void){
	startSingleADC();
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void usart2Tx_Callback(void){

}

void adcComplete_Callback(void){
	ADC_data = getADC();

	ADC_complete = 1;
}
