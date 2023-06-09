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
#include "arm_math.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "PLL.h"
#include "AdcDriver.h"
#include "PwmDriver.h"
#include "RTC_Driver.h"




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

// Elementos para la conversión analogo digital
ADC_Config_t ADC_conversion = {0};
uint16_t ADC_data[2] = {0};
uint8_t ADC_complete = 0;
uint16_t ADCi = 0;
PWM_Handler_t handlerTimerADC = {0};
uint16_t conversion1[256];
uint16_t conversion2[256];
uint8_t indicador = 0;
uint16_t k = 0;
uint8_t readyConv = 0;


// Elementos para el MOC1
GPIO_Handler_t handlerMCO1 = {0};

// Elementos para el uso del acelerómetro:
GPIO_Handler_t handlerAccelSDA = {0};
GPIO_Handler_t handlerAccelSCL = {0};
I2C_Handler_t handlerAccel = {0};
uint16_t ptrdatosAcelerometro[3] = {0};
uint16_t datosEjeX[1024] = {0};
uint16_t datosEjeY[1024] = {0};
uint16_t datosEjeZ[1024] = {0};
uint16_t muestreoAccel = 0;
uint16_t flagMuestreo = 0;
BasicTimer_Handler_t handlerTimAccel = {0};
uint16_t Acceli = 0;


float32_t datosX[1024];
float32_t* ptrdatosX;
//Elementos para la FFT
uint8_t FFTon = 0;

uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_fast_instance_f32 config_RFFT_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status status = ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize = 1024;
float32_t stopTime = 0.0;
float32_t transformedSignal[1024];


//Elementos para el uso del RTC
RTC_Handler_t handlerRTC = {0};
uint8_t hora = 0;
uint8_t minuto = 0;
uint8_t segundo = 0;





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

	//Activar el coprocesador matemático
	SCB->CPACR |= (0xF << 20);

//	i2c_readMultipleRegister(&handlerAccel, ACCEL_XOUT_H, 6, ptrdatosAcelerometro);


	while(1){

		hora = horasRTC();
		minuto = minutosRTC();
		segundo = segundosRTC();

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

		}	//Fin RxData

		//Se analizan los caracteres obtenidos
		if(stringRxComplete == 1){
			identificarComandos(bufferRx);
			stringRxComplete = 0;
		}

		if(ADC_complete == 1){
			writeMsg(&handlerCommTerminal, "Toma de datos finalizada");

			while(k < 256){
				sprintf(bufferData,"%u %u\n", conversion1[k], conversion2[k]);
				writeMsg(&handlerCommTerminal, bufferData);
				k++;
			}
			k = 0;

			ADC_complete = 0;
		}

		while(flagMuestreo == 1){
			muestreoAccel = 0;
			if(muestreoAccel == 1){
				i2c_readMultipleRegister(&handlerAccel, ACCEL_XOUT_H, 6, ptrdatosAcelerometro);
				datosEjeX[Acceli] = *(ptrdatosAcelerometro);
				datosEjeY[Acceli] = *(ptrdatosAcelerometro + 1);
				datosEjeZ[Acceli] = *(ptrdatosAcelerometro + 2);

				//Hacer un casteo
				datosX[Acceli] = (float32_t)datosEjeX[Acceli];
				Acceli++;


			}
			if(Acceli > 100){
				flagMuestreo = 0;
				writeMsg(&handlerCommTerminal, "\nFinalizado el muestreo de datos del acelerómetro\n");
			}
		}	//Fin muestreo datos acelerómetro

		if(FFTon == 1){

			//Se procede a realizar la FFT

			//Se inicializa la FFT
			statusInitFFT = arm_rfft_fast_init_f32(&config_RFFT_fast_f32, fftSize);

			if(statusInitFFT == ARM_MATH_SUCCESS){
				sprintf(bufferData, "\nInicialización exitosa\n");
				writeMsg(&handlerCommTerminal, bufferData);
			}

			//Se realiza la transformada de fourier
			stopTime = 0.0;
			int i = 0;
			int j = 0;

			sprintf(bufferData, "FFT\n");
			writeMsg(&handlerCommTerminal, bufferData);

			if(statusInitFFT == ARM_MATH_SUCCESS){

				//Se procede a calcular la FFT
				arm_rfft_fast_f32(&config_RFFT_fast_f32, datosX, transformedSignal, ifftFlag);

				arm_abs_f32(transformedSignal, datosX, fftSize);

				for(i = 1; i < fftSize; i++){
					if(i % 2){
						sprintf(bufferData, "%#.6f\n", 2*datosX[i]);
						writeMsg(&handlerCommTerminal, bufferData);
						j++;
					}
				}
			}
			else{
				writeMsg(&handlerCommTerminal, "FFT no inicializado");
			}

			FFTon = 0;


		}//Fin FFT


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
		writeMsg(&handlerCommTerminal, "\n               Menú ayuda. Información sobre los comandos:\n");

		writeMsg(&handlerCommTerminal, "\n1) MCO_clock #A        Para configurar la señal de reloj del MCO1 escribir:\n");
		writeMsg(&handlerCommTerminal, "                         A = 1 -> HSI, A = 2 -> PLL, A = 3 -> LSE\n");
		writeMsg(&handlerCommTerminal, "                         Precaución, por defecto se configura el HSI\n");

		writeMsg(&handlerCommTerminal, "\n2) MCO_prescaler #B    Para configurar el prescaler de la señal de reloj configurada para el MCO1\n");
		writeMsg(&handlerCommTerminal, "                         B = 1, 2, 3, 4, 5. (El valor 1 es no prescaler)\n");
		writeMsg(&handlerCommTerminal, "                         Precaución, por defecto no se hace prescaler a la señal\n");

		writeMsg(&handlerCommTerminal, "\n3) ADC_freq #D.        Para modificar la frecuencia de muestreo de la conversión ADC\n");
		writeMsg(&handlerCommTerminal, "                         D = 0 -> 8 KHz,  D = 1 -> 15 KHz, D = 2 -> 20 KHz\n");
		writeMsg(&handlerCommTerminal, "                         Precaución, por defecto la frecuencia es 15 KHz\n");

		writeMsg(&handlerCommTerminal, "\n4) ADC_muestreo #C.    Para activar o desactivar la conversión ADC de 2 canales\n");
		writeMsg(&handlerCommTerminal, "                         C = 1 -> Activar el muestreo, C = 0 -> Desactiva el muestreo\n");
		writeMsg(&handlerCommTerminal, "                         Precaución, por defecto el muestreo está desactivado\n");



	} //Fin comando help

	else if(strcmp(comando, "MCO_clock") == 0){
		switch (firstParameter){
		case 1:{
			config_clock_MCO1(CLOCK_SIGNAL_HSI);
			writeMsg(&handlerCommTerminal, "\nConfiguración exitosa del MCO1 como salida del HSI\n");
		break;
		}

		case 2:{
			config_clock_MCO1(CLOCK_SIGNAL_PLL);
			writeMsg(&handlerCommTerminal, "\nConfiguración exitosa del MCO1 como salida del PLL\n");

		break;
		}
		case 3:{
			config_clock_MCO1(CLOCK_SIGNAL_LSE);
			writeMsg(&handlerCommTerminal, "\nConfiguración exitosa del MCO1 como salida del LSE\n");

		break;
		}
		default:
			writeMsg(&handlerCommTerminal, "\nConfiguración de salida de reloj no válida. Se configura reloj por defecto. Ir a 'help' para mas información\n");
			config_clock_MCO1(CLOCK_SIGNAL_HSI);
			__NOP();
			break;
	}
}	//Fin comando MCO_clock

	else if(strcmp(comando, "MCO_prescaler") == 0){
		switch (firstParameter){
		case 1:{
			config_prsc_MCO1(MCO1_NOPRESCALER);
			writeMsg(&handlerCommTerminal, "\nHa configurado la señal sin prescaler");
		break;
		}

		case 2:{
			config_prsc_MCO1(MCO1_PRESCALERX2);
			writeMsg(&handlerCommTerminal, "\nHa configurado la señal con prescaler x2");

		break;
		}
		case 3:{
			config_prsc_MCO1(MCO1_PRESCALERX3);
			writeMsg(&handlerCommTerminal, "\nHa configurado la señal con prescaler x3");

		break;
		}
		case 4:{
			config_prsc_MCO1(MCO1_PRESCALERX4);
			writeMsg(&handlerCommTerminal, "\nHa configurado la señal con prescaler x4");
			break;
		}
		case 5:{
			config_prsc_MCO1(MCO1_PRESCALERX5);
			writeMsg(&handlerCommTerminal, "\nHa configurado la señal con prescaler x5");
			break;
		}
		default:
			writeMsg(&handlerCommTerminal, "\nValor de prescaler no válido. Se configura prescaler por defecto. Ir a 'help' para más información\n");
			config_prsc_MCO1(MCO1_NOPRESCALER);
			__NOP();
			break;
	}
}	//Fin comando MCO_prescaler


	else if(strcmp(comando, "ADC_freq") == 0){
		switch (firstParameter){
		case 0:{
			handlerTimerADC.config.periodo			= 125;	//Frecuencia de muestreo de 8 KHz
			handlerTimerADC.config.duttyCicle		= 63;	//Dutty del 50%
			writeMsg(&handlerCommTerminal, "\nFrecuencia de muestreo de 8 KHz\n");
		break;
		}

		case 1:{
			handlerTimerADC.config.periodo			= 66;	//Frecuencia de muestreo de aproximadamente 15 KHz
			handlerTimerADC.config.duttyCicle		= 33;	//Dutty del 50%
			writeMsg(&handlerCommTerminal, "\nFrecuencia de muestreo de 15 KHz\n");

		break;
		}
		case 2:{
			handlerTimerADC.config.periodo			= 25;	//Frecuencia de muestreo de 40 KHz
			handlerTimerADC.config.duttyCicle		= 13;	//Duty del 50%
			writeMsg(&handlerCommTerminal, "\nFrecuencia de muestreo de 40 KHz\n");

		break;
		}
		default:
			writeMsg(&handlerCommTerminal, "\nConfiguración de frecuencia no válida. Se configura frecuencia por defecto. Ir a 'help' para más información\n");
			handlerTimerADC.config.periodo			= 66;	//Frecuencia de muestreo de aproximadamente 15 KHz
			handlerTimerADC.config.duttyCicle		= 33;	//Dutty del 50%
			__NOP();
			break;
	}
}	//Fin comando ADC_freq

	else if(strcmp(comando, "ADC_muestreo") == 0){

		if(firstParameter == 1){
			writeMsg(&handlerCommTerminal, "\nIniciando la conversión ADC a dos canales\n");
			//Activar el PWM para que se genere la conversión
			startPwmSignal(&handlerTimerADC);
			enableOutput(&handlerTimerADC);
		}
		else if(firstParameter == 0){
			writeMsg(&handlerCommTerminal, "\nFinalizando la conversión ADC\n");
			//Desactivar el PWM, por lo que no se genera la conversión
			disableOutput(&handlerTimerADC);
			stopPwmSignal(&handlerTimerADC);
		}
		else{
			writeMsg(&handlerCommTerminal, "\nComando erróneo. No se inicia la conversión\n");
			//Desactivar el PWM, por lo que no se genera la conversión
			disableOutput(&handlerTimerADC);
			stopPwmSignal(&handlerTimerADC);
		}
	}	//Fin comando ADC_muestreo

	else if(strcmp(comando, "Accel_ON") == 0){
		writeMsg(&handlerCommTerminal, "\nComenzando captura de datos del acelerómetro\n");
		flagMuestreo = 1;
	}

	else if(strcmp(comando, "FFT_on") == 0){
		writeMsg(&handlerCommTerminal, "\nRealizando transformada rápida de Fourier\n");
		FFTon = 1;
	}



}	//Fin indentificarComandos

void init_hardware(void){

	/*
	 * 			Para poder utilizar el pin PH0 como led de estado:
	 * En los pines PH0 y PH1 usualmente se encuentran las entradas y salidas del oscilador HSE
	 * Para poder usarlos como GPIO, es necesario apagar el oscilador HSE (después de reset, se
	 * mantiene apagado)

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

	ADC_conversion.channel[0] = ADC_CHANNEL_0;
	ADC_conversion.channel[1] = ADC_CHANNEL_1;
	ADC_conversion.dataAlignment = ADC_ALIGNMENT_RIGHT;
	ADC_conversion.samplingPeriod[0] = ADC_SAMPLING_PERIOD_112_CYCLES;
	ADC_conversion.samplingPeriod[1] = ADC_SAMPLING_PERIOD_112_CYCLES;
	ADC_conversion.resolution = ADC_RESOLUTION_12_BIT;
	ADC_conversion.EXT_edge = ADC_EDGETYPE_RISING;
	ADC_conversion.EXT_sel = ADC_EXTSEL_TIM5_CH3;

	ADC_ConfigMultichannel(&ADC_conversion, 2);

	handlerTimerADC.ptrTIMx					= TIM5;
	handlerTimerADC.config.channel			= PWM_CHANNEL_3;
	handlerTimerADC.config.prescaler		= 100;	//Doy pasos de 1us
	handlerTimerADC.config.periodo			= 66;	//Frecuencia de muestreo de aproximadamente 15 KHz
	handlerTimerADC.config.duttyCicle		= 33;	//Duty del 50%

	pwm_Config(&handlerTimerADC);
//	startPwmSignal(&handlerTimerADC);
//	enableOutput(&handlerTimerADC);

	handlerRTC.Am_pm = PM;
	handlerRTC.año = 23;
	handlerRTC.mes = 6;
	handlerRTC.dia = 8;
	handlerRTC.diaSemana = JUEVES;
	handlerRTC.formatoHora = FORMATO_12HORAS;
	handlerRTC.hora = 7;
	handlerRTC.minuto = 25;
	handlerRTC.segundo = 0;

	configRTC(&handlerRTC);

	handlerTimAccel.ptrTIMx								= TIM3;
	handlerTimAccel.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimAccel.TIMx_Config.TIMx_speed				= BTIMER_SPEED_500us_100MHz;
	handlerTimAccel.TIMx_Config.TIMx_period				= 10;
	handlerTimAccel.TIMx_Config.TIMx_interruptEnable	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerTimAccel);

}

// Callback para el led de estado

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLedOK);
}


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
void adcComplete_Callback(void){

	if(indicador == 0){
		conversion1[ADCi] = getADC();
		indicador = 1;
	}
	else{
		conversion2[ADCi] = getADC();
		indicador = 0;
		ADCi++;

	}

	if(ADCi >= 256){
		ADC_complete = 1;
		ADCi = 0;
		disableOutput(&handlerTimerADC);
		stopPwmSignal(&handlerTimerADC);
	}
}

//Callback del timer del acelerómetro
void BasicTimer3_Callback(void){
	muestreoAccel = 1;
}

