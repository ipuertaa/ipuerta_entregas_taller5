/*
 * Sln_examen.c
 *
 *  Created on: Jun 2, 2023
 *      Author: isabel
 */


#include <stm32f4xx.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "PLL.h"
#include "AdcDriver.h"
#include "PwmDriver.h"


// Elementos para el led de estado PH1 + TIM2
BasicTimer_Handler_t handlerStateOKTimer = {0};
GPIO_Handler_t handlerLedOK = {0};


// Elementos para la comunicación serial USART1 y el manejo de comandos
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerCommTerminal = {0};

uint8_t rxData = 0;
char bufferRx[100];
char bufferData[100];
uint16_t counterRx = 0;
uint8_t stringRxComplete = 0;
char comando[64];
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
char userMsg[100];

//// Elementos para la conversión analogo digital
//ADC_Config_t ADC_conversion = {0};
//uint16_t ADC_data[2] = {0};
//uint8_t ADC_complete = 0;
//uint8_t ADCi = 0;
////PWM_Handler_t handlerTimerADC = {0};


// Elementos para el MOC1
GPIO_Handler_t handlerMCO1 = {0};

// Elementos para el uso del acelerómetro:
GPIO_Handler_t handlerAccelSDA = {0};
GPIO_Handler_t handlerAccelSCL = {0};
I2C_Handler_t handlerAccel = {0};
uint16_t ptrdatosAcelerometro[3] = {0};

//// Ensayos ADC unicanal
//BasicTimer_Handler_t handlerTIM5 = {0};
//ADC_Config_t_uni chanel0 = {0};
//uint16_t datosADC_prueba = 0;



#define ACCEL_ADDRESS		0b1101001
#define ACCEL_XOUT_H	59
#define ACCEL_XOUT_L	60
#define ACCEL_YOUT_H	61
#define ACCEL_YOUT_L	62
#define ACCEL_ZOUT_H	63
#define ACCEL_ZOUT_L	64

#define PWR_MGMT_1 107
#define WHO_AM_I 117
#define AFS_SEL	28
#define GRAVEDAD	9.8


//Definición de cabeceras de funciones
void identificarComandos (char *ptrbufferRx);
void init_hardware(void);
void init_acelerometro(void);


int main(void){

	configPLL(PLL_FRECUENCIA_100MHZ);
	init_hardware();
	writeMsg(&handlerCommTerminal, "Examen Taller 5 Isabel Puerta Alvarez\n");
	init_acelerometro();
	writeMsg(&handlerCommTerminal, "\nSistema inicializado con exito\n");

	i2c_readMultipleRegister(&handlerAccel, ACCEL_XOUT_H, 6, ptrdatosAcelerometro);


	while(1){

//		if(ADC_complete == 1){
//			sprintf(bufferData, "Data canal 1: %u\n", datosADC_prueba);
//			writeMsg(&handlerCommTerminal, bufferData);
////			sprintf(bufferData, "\nData canal 2: %u\n", ADC_data[1]);
////			writeMsg(&handlerCommTerminal, bufferData);
//			ADC_complete = 0;
//		}



		if(rxData != '\0'){


			//Se comienza a almacenar la cadena de caracteres recibida
			bufferRx[counterRx] = rxData;
			counterRx++;

			//Se evalúa el final de la cadena de caracteres
			if(rxData == '@'){
				stringRxComplete = 1;

				//Se agrega el caracter nulo al final del string
				bufferRx[counterRx] = '\0';

				// Se detiene el contador
				counterRx = 0;
			}

			//Se detiene el almacenamiento
			rxData = '\0';
		}	//Fin if rxData != 0


		//Se analizan los caracteres obtenidos
		if(stringRxComplete == 1){
			identificarComandos(bufferRx);
			stringRxComplete = 0;
		}

	}	//Fin while
}	//Fin main














































void init_acelerometro(void){

	/*
	 * El registro WHO AM I, se usa para identificar el dispositivo.
	 * Debo recibir 0b1101001 (6 bits, no se tiene en cuenta el del ADO)
	 * EL valor por defecto es 0x68
	 *
	 * En el PWR-MGMT_1 se controla la alimentación.
	 * Si SLEEP = 1 -> modo de ahorro de energia
	 * Si CYCLE = 1 y SLEEP = 0 -> entra en modo ciclo
	 * Si DEVICE_RESET = 1; todos los registros se van a sus valores
	 * por defecto, luego se vuelve automaticamente a cero
	 * Por defecto el accel está en SLEEP, por lo que hay que, hay que hacer
	 * cero el registro 107 para despertarlo
	 */

	uint8_t SlaveAdress = i2c_readSingleRegister(&handlerAccel, WHO_AM_I);
	i2c_writeSingleRegister(&handlerAccel, PWR_MGMT_1, 0x00);
	uint8_t PWR_STATE = i2c_readSingleRegister(&handlerAccel, PWR_MGMT_1);

	if((SlaveAdress == 0x68) && (PWR_STATE == 0x00)){
		sprintf(bufferData, "\nAcelerometro inicializado\n");
		writeMsg(&handlerCommTerminal, bufferData);
	}

}





void identificarComandos (char *ptrbufferRx){

	/*
	 * Lee la cadena de caracteres
	 * Mediante la función sscanf (opuesta al sprintf) se divide la cadena
	 * de caracteres en diferentos elementos
	 * Esto es para recibir frases completas desde el puerto serial
	 */

	sscanf(ptrbufferRx, "%s %u %u %s" , comando, &firstParameter, &secondParameter, userMsg);

	if(strcmp(comando, "help") == 0){
		writeMsg(&handlerCommTerminal, "Menú ayuda. Información sobre los comandos:\n");
		writeMsg(&handlerCommTerminal, "\n1) MCOconfig #A #B Para configurar el MCO1 escribir \n");
		writeMsg(&handlerCommTerminal, "                   A = 1 -> HSI, A = 2 -> PLL, A = 3 -> LSE\n");
		writeMsg(&handlerCommTerminal, "                   B = 1, 2, 3, 4, 5 (prescaler)\n");

	}
	else if(strcmp(comando, "MCOconfig") == 0){
		switch (firstParameter){
		case 1:{
			if((secondParameter <= 5) && (secondParameter != 0)){
				configMCO1(CLOCK_SIGNAL_HSI, (uint8_t)secondParameter);
				writeMsg(&handlerCommTerminal, "\nConfiguración exitosa del MCO1 como salida del HSI");
			}
			else{
				writeMsg(&handlerCommTerminal, "\nValor de prescaler no permitido");
			}
		break;
		}

		case 2:{
			if((secondParameter <= 5) && (secondParameter != 0)){
				configMCO1(CLOCK_SIGNAL_PLL, (uint8_t)secondParameter);
				writeMsg(&handlerCommTerminal, "\nConfiguración exitosa del MCO1 como salida del PLL");
			}
			else{
				writeMsg(&handlerCommTerminal, "\nValor de prescaler no permitido");
			}
		break;
		}
		case 3:{
			if((secondParameter <= 5) && (secondParameter != 0)){
				configMCO1(CLOCK_SIGNAL_LSE, (uint8_t)secondParameter);
				writeMsg(&handlerCommTerminal, "\nConfiguración exitosa del MCO1 como salida del LSE");
			}
			else{
				writeMsg(&handlerCommTerminal, "\nValor de prescaler no permitido");
			}
		break;
		}
		default:
			writeMsg(&handlerCommTerminal, "\nConfiguración de salida de reloj no válida.");
			__NOP();
			break;


	}


}	//FIn identificarComandos

}

void init_hardware(void){

	/*
	 * 			Para poder utilizar el pin PH0 como led de estado
	 *
	 * En los pines PH0 y PH1 usualmente se encuentran las entradas y salidas del oscilador HSE
	 * Para poder usarlos como GPIO, es necesario apagar el oscilador HSE (después de reset, se
	 * mantiene apagado)
	 *
	 * En el RCC_RC:
	 * 		- HSEON
	 *
	 *
	 */
	RCC->CR &= ~RCC_CR_HSEON;

	//configurar LED2 - PH1 - LED DE ESTADO

	handlerLedOK.pGPIOx 										= GPIOH;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber 					= PIN_1;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedOK);

	handlerStateOKTimer.ptrTIMx  								= TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed 					= BTIMER_SPEED_500us_100MHz;
	handlerStateOKTimer.TIMx_Config.TIMx_period 				= 500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable 		= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerStateOKTimer);


	//Configuración de la comunicación serial USART1

	handlerPinTX.pGPIOx 								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx 								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerPinRX);

	handlerCommTerminal.ptrUSARTx 						= USART1;
	handlerCommTerminal.USART_Config.USART_baudrate 	=  USART_BAUDRATE_100MHz_19200;
	handlerCommTerminal.USART_Config.USART_datasize 	= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_mode 		= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_enableIntRX 	= USART_RX_INTERRUPT_ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX 	= USART_TX_INTERRUPT_ENABLE;

	USART_Config(&handlerCommTerminal);

	//Configuración del pin MOC1

//	//Limpiar los bits correspondientes == Seleccionar HSI como salida
//	RCC->CFGR &= ~(RCC_CFGR_MCO1);
//
//	//Limpiar los bits del prescaler == no escalar
//	RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);

	//Configurar el pin PA8 con la función alternativa AF0

	handlerMCO1.pGPIOx							= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber 	= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed 	= GPIO_OSPEED_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;

	GPIO_Config(&handlerMCO1);

	// Configurar los elementos para el manejo del acelerómetro
	handlerAccelSCL.pGPIOx		= GPIOB;
	handlerAccelSCL.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerAccelSCL.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerAccelSCL.GPIO_PinConfig.GPIO_PinOPType   = GPIO_OTYPE_OPENDRAIN;
	handlerAccelSCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerAccelSCL.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerAccelSCL.GPIO_PinConfig.GPIO_PinAltFunMode	= AF4;

	GPIO_Config(&handlerAccelSCL);

	handlerAccelSDA.pGPIOx 								= GPIOB;
	handlerAccelSDA.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerAccelSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerAccelSDA.GPIO_PinConfig.GPIO_PinOPType   	= GPIO_OTYPE_OPENDRAIN;
	handlerAccelSDA.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerAccelSDA.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerAccelSDA.GPIO_PinConfig.GPIO_PinAltFunMode	= AF4;

	GPIO_Config(&handlerAccelSDA);


	handlerAccel.ptrI2Cx		= I2C1;
	handlerAccel.modeI2C		= I2C_MODE_FM;
	handlerAccel.slaveAddress	= ACCEL_ADDRESS;
	handlerAccel.Clock_Freq		= CLOCK_FREQ_100MHz;

	i2c_config(&handlerAccel);

//	ADC_conversion.channel[0] = ADC_CHANNEL_0;
//	ADC_conversion.channel[1] = ADC_CHANNEL_1;
//	ADC_conversion.dataAlignment = ADC_ALIGNMENT_RIGHT;
//	ADC_conversion.samplingPeriod[0] = ADC_SAMPLING_PERIOD_84_CYCLES;
//	ADC_conversion.samplingPeriod[1] = ADC_SAMPLING_PERIOD_84_CYCLES;
//	ADC_conversion.resolution = ADC_RESOLUTION_12_BIT;
//	ADC_conversion.EXT_edge = ADC_EDGETYPE_RISING;
//	ADC_conversion.EXT_sel = ADC_EXTSEL_TIM5_CH3;
//
//	ADC_ConfigMultichannel(&ADC_conversion, 2);
//
//	handlerTimerADC.ptrTIMx					= TIM5;
//	handlerTimerADC.config.channel			= PWM_CHANNEL_3;
//	handlerTimerADC.config.prescaler		= BTIMER_SPEED_500us_100MHz;
//	handlerTimerADC.config.periodo			= 100;
//	handlerTimerADC.config.duttyCicle		= 50;
//
//	pwm_Config(&handlerTimerADC);
//	startPwmSignal(&handlerTimerADC);
//	enableOutput(&handlerTimerADC);

//	handlerTIM5.ptrTIMx  								= TIM5;
//	handlerTIM5.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
//	handlerTIM5.TIMx_Config.TIMx_speed 					= BTIMER_SPEED_500us_100MHz;
//	handlerTIM5.TIMx_Config.TIMx_period 				= 100;
//	handlerTIM5.TIMx_Config.TIMx_interruptEnable 		= BTIMER_INTERRUPT_ENABLE;
//
//	BasicTimer_Config(&handlerTIM5);
//
//	chanel0.channel = ADC_CHANNEL_0;
//	chanel0.dataAlignment = ADC_ALIGNMENT_RIGHT;
//	chanel0.samplingPeriod = ADC_SAMPLING_PERIOD_84_CYCLES;
//	chanel0.resolution = ADC_RESOLUTION_12_BIT;
//
//	adc_Config(&chanel0);

}

// Callback para el led de estado

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLedOK);


}
//
//void BasicTimer5_Callback(void){
//	startSingleADC();
//
//}

// Callback para la comunicación serial USART1

// Callback para la recepción del USART1
void usart1Rx_Callback(void){
	//Se lee el dato del DR y se almacena. Al hacer esto se baja la bandera de la interrupción
	rxData = getRxData();
}

//Callback para la transmisión del USART1
void usart1Tx_Callback(void){

}

//Callback del ADC
//void adcComplete_Callback(void){
//
////	//Guardo los datos de las dos conversiones
////	ADC_data[ADCi] = getADC();
////	ADCi++;
////	if(ADCi > 1){
////		ADCi = 0;
////	}
//	datosADC_prueba = getADC();
//	ADC_complete = 1;
//
//
//
//}

