/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez

	Programación del teclado matricial
 ******************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "OledDriver.h"
#include "I2CDriver.h"
#include "PwmDriver.h"





/* 4 pines de propósito general configurados como columnas serán salidas a un nivel alto.
 * 4 filas serán configuradas como entradas y serán las que causarán la interrupción.
 *
 */

//Definición de variables

uint8_t banderaBarrido = 0;
uint8_t banderaCondicion = 1;
uint8_t flagFilas  = 0;
uint8_t flagMoneda = 0;
uint16_t dinero = 0;

#define PRECIO_CELDA1  	1000
#define PRECIO_CELDA2  	2500
#define PRECIO_CELDA3  	500
#define PRECIO_CELDA4  	3500
#define oLEDAddress 	0x3C
#define PASOS_SERVO1	70
#define PASOS_SERVO2	80
#define PASOS_SERVO3	70
#define PASOS_SERVO4	80

char boton = 10;
char casilla[2] = {0};
uint8_t llenarCasilla = 0;
uint8_t casillaCompleta = 0;
uint8_t contenidoOK = 0;

uint16_t conteoServo = 0;



//Definición de los handler

//Handlers del GPIO: Todos los pines, independientes de entrada o salida, se deben configurar

GPIO_Handler_t handlerColumna1 = {0};		//1,4,7,*
GPIO_Handler_t handlerColumna2 = {0};		//2,5,8,0
GPIO_Handler_t handlerColumna3 = {0};		//3,6,9,#
GPIO_Handler_t handlerColumna4 = {0};		//A,B,C,D

GPIO_Handler_t handlerFila1 = {0};			//1,2,3
GPIO_Handler_t handlerFila2 = {0};			//4,5,6
GPIO_Handler_t handlerFila3 = {0};			//7,8,9
GPIO_Handler_t handlerFila4 = {0};			//*,0,#

GPIO_Handler_t handlerLED2 					= {0};

//Handler para el timer: con esto podré controlar el barrido de las columnas

BasicTimer_Handler_t handlerBarrido = {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};

//Handlers para los EXTI. Cada fila es una entrada por donde podrá haber una interrupción o no

EXTI_Config_t handlerEXTI1 = {0};		//exti para la fila 1
EXTI_Config_t handlerEXTI2 = {0};		//exti para la fila 2
EXTI_Config_t handlerEXTI3 = {0};		//exti para la fila 3
EXTI_Config_t handlerEXTI4 = {0};		//exti para la fila 4


//Handler para los elementos asociados a la recepción de las monedas
GPIO_Handler_t handlerRecepMonedas = {0};
EXTI_Config_t handlerEXTI5_monedas = {0}; 	//PB5

//Elementos para la comunicación serial
USART_Handler_t handlerUsart2			= {0};
GPIO_Handler_t handlerPinTX 			= {0};
GPIO_Handler_t handlerPinRX 			= {0};
uint8_t usart2DataReceived = 0;
char bufferMsg[100] = {0};

//Elementos para el manejo de la pantalla OLED
GPIO_Handler_t handlerOledSDA = {0};
GPIO_Handler_t handlerOledSCL = {0};
I2C_Handler_t handlerOLED = {0};

//Elementos para el manejo de los motores
BasicTimer_Handler_t timerVueltasServo = {0};
//Elementos para el PWM
GPIO_Handler_t handlerMotor1			= {0};
PWM_Handler_t handlerPWM_Motor1			= {0};

GPIO_Handler_t handlerMotor2			= {0};
PWM_Handler_t handlerPWM_Motor2			= {0};

GPIO_Handler_t handlerMotor3			= {0};
PWM_Handler_t handlerPWM_Motor3			= {0};

GPIO_Handler_t handlerMotor4			= {0};
PWM_Handler_t handlerPWM_Motor4			= {0};
uint8_t motorActivo = 0;

char dummyMsg[64] = {0};

// Cabeceras de funciones

void init_hardware(void);		//Función para la configuración de pines.
void identificarF1(void);
void identificarF2(void);
void identificarF3(void);
void identificarF4(void);
void limpiar_casilla(void);

int main(void){

	init_hardware();
	delay_ms(10);
	initOled(&handlerOLED);
	delay_ms(10);
//	sprintf(bufferMsg, "  BIENVENIDO       OPRIMA BOTON");
	OLED_print_msg(&handlerOLED, "BIENVENIDO $ : =");


	while(1){

		if(boton != 10){

			switch (boton){
			case 1:{
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "CASILLA %u ACTIVA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, bufferMsg);

				delay_ms(2000);
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "INGRESE $%u", PRECIO_CELDA1);
				OLED_print_msg_pag(&handlerOLED, 3, bufferMsg);
				dinero = 0;
				while(flagMoneda != 1){
					__NOP();
				}
//				flagMoneda =0;

				while (dinero < PRECIO_CELDA1) {
					if(flagMoneda){
						dinero += 500;
						clearScreenOLED(&handlerOLED);
						sprintf(bufferMsg, "DINERO INGRESADO = $ %u ", dinero);
						OLED_print_msg(&handlerOLED, bufferMsg);
						flagMoneda =0;
					}
				}
				delay_ms(1000);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "ENTREGANDO PRODUCTO");
				enableOutput(&handlerPWM_Motor1);
				conteoServo = 0;

				while (conteoServo < PASOS_SERVO1) {
					__NOP();
				}
				disableOutput(&handlerPWM_Motor1);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "GRACIAS POR SU COMPRA");
				delay_ms(1000);
				OLED_print_msg(&handlerOLED, "BIENVENIDO SELECCIONE UNA CASILLA");
				flagMoneda = 0;
				break;
				}

			case 2:{
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "CASILLA %u ACTIVA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, bufferMsg);

				delay_ms(2000);
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "INGRESE $%u", PRECIO_CELDA2);
				OLED_print_msg_pag(&handlerOLED, 3, bufferMsg);
				dinero = 0;

				while(flagMoneda != 1){
					__NOP();
				}

				while(dinero < PRECIO_CELDA2){

					if(flagMoneda){
						dinero+= 500;
						clearScreenOLED(&handlerOLED);
						sprintf(bufferMsg, "DINERO INGRESADO = $ %u ", dinero);
						OLED_print_msg(&handlerOLED, bufferMsg);
						flagMoneda = 0;
					}
				}
				delay_ms(1000);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "ENTREGANDO PRODUCTO");
				enableOutput(&handlerPWM_Motor2);
				conteoServo = 0;


				while (conteoServo < PASOS_SERVO2){
					__NOP();
				}

				disableOutput(&handlerPWM_Motor2);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "GRACIAS POR SU COMPRA");
				delay_ms(1000);
				OLED_print_msg(&handlerOLED, "BIENVENIDO SELECCIONE UNA CASILLA");
				flagMoneda = 0;
				break;
			}
			case 3:{
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "CASILLA %u ACTIVA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, bufferMsg);


				delay_ms(2000);
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "INGRESE $%u", PRECIO_CELDA3);
				OLED_print_msg_pag(&handlerOLED, 3, bufferMsg);
				dinero = 0;

				while(flagMoneda != 1){
					__NOP();
				}

				while(dinero < PRECIO_CELDA3){
					if(flagMoneda){
						dinero+= 500;
						clearScreenOLED(&handlerOLED);
						sprintf(bufferMsg, "DINERO INGRESADO = $ %u", dinero);
						OLED_print_msg(&handlerOLED, bufferMsg);
						flagMoneda = 0;
					}
				}
				delay_ms(1000);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "ENTREGANDO PRODUCTO");
				enableOutput(&handlerPWM_Motor3);
				conteoServo = 0;

				while(conteoServo < PASOS_SERVO3){
					__NOP();
				}

				disableOutput(&handlerPWM_Motor3);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "GRACIAS POR SU COMPRA");
				delay_ms(1000);
				OLED_print_msg(&handlerOLED, "BIENVENIDO SELECCIONE UNA CASILLA");
				flagMoneda = 0;
				break;

			}

			case 4:{
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "CASILLA %u ACTIVA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, bufferMsg);

				delay_ms(2000);
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "INGRESE $%u", PRECIO_CELDA4);
				OLED_print_msg_pag(&handlerOLED, 3, bufferMsg);
				dinero = 0;

				while(flagMoneda != 1){
					__NOP();
				}

				while(dinero < PRECIO_CELDA4){
					if(flagMoneda){
						dinero+= 500;
						clearScreenOLED(&handlerOLED);
						sprintf(bufferMsg, "DINERO INGRESADO = $ %u", dinero);
						OLED_print_msg(&handlerOLED, bufferMsg);
						flagMoneda = 0;
					}
				}
				delay_ms(1000);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "ENTREGANDO PRODUCTO");
				enableOutput(&handlerPWM_Motor4);
				conteoServo = 0;

				while(conteoServo < PASOS_SERVO4){
					__NOP();
				}
				disableOutput(&handlerPWM_Motor4);
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "GRACIAS POR SU COMPRA");
				delay_ms(1000);
				OLED_print_msg(&handlerOLED, "BIENVENIDO SELECCIONE UNA CASILLA");
				flagMoneda = 0;
				break;
			}
			case 5:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "CASILLA %u NA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, dummyMsg);
				break;
			}
			case 6:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "CASILLA %u NA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, dummyMsg);
				break;
			}

			case 7:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "CASILLA %u NA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, dummyMsg);
				break;
			}
			case 8:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "CASILLA %u NA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, dummyMsg);
				break;
			}
			case 9:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "CASILLA %u NA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, dummyMsg);
				break;
			}
			case 0:{
				clearScreenOLED(&handlerOLED);
				sprintf(dummyMsg, "CASILLA %u NA", boton);
				OLED_print_msg_pag(&handlerOLED, 2, dummyMsg);
				break;
			}
			case '*':{
				clearScreenOLED(&handlerOLED);
				sprintf(bufferMsg, "CASILLA * ACTIVA");
				OLED_print_msg_pag(&handlerOLED, 2, bufferMsg);
				break;

			}

			default:{
				clearScreenOLED(&handlerOLED);
				OLED_print_msg(&handlerOLED, "ERROR");
				break;
			}

			}
			boton = 10;
			delay_ms(30);

		}	//Fin if boton


		if(banderaCondicion == 0){
			switch(banderaBarrido){
			case 1:{
				GPIO_WritePin(&handlerColumna1, SET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, RESET);
				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;
			}
			case 2: {
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, SET);
				GPIO_WritePin(&handlerColumna3, RESET);
				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;
			}
			case 3:{
				GPIO_WritePin(&handlerColumna1, RESET);
				GPIO_WritePin(&handlerColumna2, RESET);
				GPIO_WritePin(&handlerColumna3, SET);
				GPIO_WritePin(&handlerColumna4, RESET);
				banderaCondicion = 1;
				break;

			}
			case 4:{
				banderaCondicion = 1;
				banderaBarrido = 0;
				break;
			}
			default:{
				__NOP();
				break;
			}


			} //Fin switch
		} 	//Fin Barrido de columnas

		else{
			__NOP();
		}


	}	//Fin while

}	//Fin main



//		DEFINICIÓN DE LAS FUNCIONES

void init_hardware(void){

	//Configurar la C1

	handlerColumna1.pGPIOx 								= GPIOA;
	handlerColumna1.GPIO_PinConfig.GPIO_PinNumber 		= PIN_15;
	handlerColumna1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna1.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna1.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna1);		//Cargar la configuración
//	GPIO_WritePin(&handlerColumna1, RESET);


	//Configurar la C2 -> PB0

	handlerColumna2.pGPIOx 								= GPIOB;
	handlerColumna2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_0;
	handlerColumna2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna2);		//Cargar la configuración
//	GPIO_WritePin(&handlerColumna2, RESET);


	//Configurar la C3 -> PC13

	handlerColumna3.pGPIOx 								= GPIOC;
	handlerColumna3.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerColumna3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna3.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna3.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna3);		//Cargar la configuración
//	GPIO_WritePin(&handlerColumna3, RESET);

	//Configurar la C4 -> PC2

	handlerColumna4.pGPIOx 								= GPIOC;
	handlerColumna4.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerColumna4.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerColumna4.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerColumna4.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerColumna4.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerColumna4);		//Cargar la configuración


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

	//Configurar el TIM4 para hacer el barrido

	handlerBarrido.ptrTIMx 							= TIM4;
	handlerBarrido.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerBarrido.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerBarrido.TIMx_Config.TIMx_period 			= 4;
	handlerBarrido.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBarrido);

/*  CONFIGURACIÓN DE LAS FILAS   */

	//Configuración del pin para la F1 ->PC3

	handlerFila1.pGPIOx 								= GPIOC;
	handlerFila1.GPIO_PinConfig.GPIO_PinNumber 			= PIN_3;
	handlerFila1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila1.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI1.pGPIOHandler = &handlerFila1;
	handlerEXTI1.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F1
	extInt_Config(&handlerEXTI1);


	//Configuración del pin para la F2 ->PA4

	handlerFila2.pGPIOx 								= GPIOA;
	handlerFila2.GPIO_PinConfig.GPIO_PinNumber 			= PIN_4;
	handlerFila2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila2.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI2.pGPIOHandler = &handlerFila2;
	handlerEXTI2.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F2
	extInt_Config(&handlerEXTI2);



	//Configuración del pin para la F3 ->PC1

	handlerFila3.pGPIOx 								= GPIOC;
	handlerFila3.GPIO_PinConfig.GPIO_PinNumber 			= PIN_1;
	handlerFila3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila3.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila3.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI3.pGPIOHandler = &handlerFila3;
	handlerEXTI3.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F3
	extInt_Config(&handlerEXTI3);


	//Configuración del pin para la F4 ->PC0

	handlerFila4.pGPIOx 								= GPIOC;
	handlerFila4.GPIO_PinConfig.GPIO_PinNumber 			= PIN_0;
	handlerFila4.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerFila4.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerFila4.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLDOWN;

	//Configuración del EXTI

	handlerEXTI4.pGPIOHandler = &handlerFila4;
	handlerEXTI4.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Cargar la configuración del exti y GPIO para la F4
	extInt_Config(&handlerEXTI4);


	/*  CONFIGURACIÓN DE LOS ELEMENTOS ASOCIADOS A LA RECEPCIÓN DE LAS MONEDAS */

	handlerRecepMonedas.pGPIOx 									= GPIOB;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerRecepMonedas.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	//Configuración del EXTI

	handlerEXTI5_monedas.pGPIOHandler = &handlerRecepMonedas;
	handlerEXTI5_monedas.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;

	//Cargar la configuración del exti y GPIO
	extInt_Config(&handlerEXTI5_monedas);

	//Configuración de la comunicación serial:

	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	handlerUsart2.ptrUSARTx 						= USART2;
	handlerUsart2.USART_Config.USART_baudrate 		= USART_BAUDRATE_19200;
	handlerUsart2.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_mode 			= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_enableIntRX 	= USART_RX_INTERRUPT_ENABLE;
	handlerUsart2.USART_Config.USART_enableIntTX 	= USART_TX_INTERRUPT_ENABLE;

	USART_Config(&handlerUsart2);

	//Inicialización del periférico SysTick
	config_SysTick_ms(HSI_CLOCK_CONFIGURED);

	// Configurar los elementos para el manejo de la pantalla OLED

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

	//Configuración de los elementos de los motores

	//Configurar el TIM4 para hacer el barrido

	timerVueltasServo.ptrTIMx 							= TIM5;
	timerVueltasServo.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	timerVueltasServo.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	timerVueltasServo.TIMx_Config.TIMx_period 			= 20;
	timerVueltasServo.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&timerVueltasServo);

	// Motor 1.
	handlerMotor1.pGPIOx								= GPIOC;
	handlerMotor1.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerMotor1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerMotor1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerMotor1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMotor1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerMotor1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;

	GPIO_Config(&handlerMotor1);

	//Configurar el timer para que genere la señal PWM
	handlerPWM_Motor1.ptrTIMx							= TIM3;
	handlerPWM_Motor1.config.channel					= PWM_CHANNEL_1;
	handlerPWM_Motor1.config.periodo					= 20000;
	handlerPWM_Motor1.config.prescaler					= 16;
	handlerPWM_Motor1.config.duttyCicle					= 1000;

	pwm_Config(&handlerPWM_Motor1);
	startPwmSignal(&handlerPWM_Motor1); //habilito todo el periferico

	//Activamos la señal del motor 1
	disableOutput(&handlerPWM_Motor1);
//	startPwmSignal(&handlerPWM_Motor1);


//	//Motor 2
	handlerMotor2.pGPIOx								= GPIOC;
	handlerMotor2.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerMotor2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerMotor2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerMotor2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMotor2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerMotor2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;

	GPIO_Config(&handlerMotor2);

	//Configurar el timer para que genere la señal PWM
	handlerPWM_Motor2.ptrTIMx							= TIM3;
	handlerPWM_Motor2.config.channel					= PWM_CHANNEL_4;
	handlerPWM_Motor2.config.periodo					= 20000;
	handlerPWM_Motor2.config.prescaler					= 16;
	handlerPWM_Motor2.config.duttyCicle					= 1000;

	pwm_Config(&handlerPWM_Motor2);

	//Activamos la señal del motor 2
	disableOutput(&handlerPWM_Motor2);
//	startPwmSignal(&handlerPWM_Motor2);


	//Motor 3
	handlerMotor3.pGPIOx								= GPIOC;
	handlerMotor3.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerMotor3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerMotor3.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerMotor3.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMotor3.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerMotor3.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;

	GPIO_Config(&handlerMotor3);

	//Configurar el timer para que genere la señal PWM
	handlerPWM_Motor3.ptrTIMx							= TIM3;
	handlerPWM_Motor3.config.channel					= PWM_CHANNEL_3;
	handlerPWM_Motor3.config.periodo					= 20000;
	handlerPWM_Motor3.config.prescaler					= 16;
	handlerPWM_Motor3.config.duttyCicle					= 1000;

	pwm_Config(&handlerPWM_Motor3);

	//Activamos la señal del motor 3
	disableOutput(&handlerPWM_Motor3);
//	startPwmSignal(&handlerPWM_Motor3);



//	//Motor 4
	handlerMotor4.pGPIOx								= GPIOC;
	handlerMotor4.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerMotor4.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerMotor4.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerMotor4.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMotor4.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerMotor4.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;

	GPIO_Config(&handlerMotor4);

	//Configurar el timer para que genere la señal PWM
	handlerPWM_Motor4.ptrTIMx							= TIM3;
	handlerPWM_Motor4.config.channel					= PWM_CHANNEL_2;
	handlerPWM_Motor4.config.periodo					= 20000;
	handlerPWM_Motor4.config.prescaler					= 16;
	handlerPWM_Motor4.config.duttyCicle					= 1000;

	pwm_Config(&handlerPWM_Motor4);

	disableOutput(&handlerPWM_Motor4);






}




void BasicTimer4_Callback(void){
	banderaBarrido++;
	banderaCondicion = 0;

}

void callback_extInt3(void){	// F1
	identificarF1();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void callback_extInt4(void){	//F2
	identificarF2();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void callback_extInt1(void){	//F3
	identificarF3();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void callback_extInt0(void){	//F4
	identificarF4();
//	if(llenarCasilla == 1){
//		casilla[0] = boton;
//		casilla1OK = 1;
//		llenarCasilla = 0;
//	}
//	else if(casilla1OK == 1){
//		casilla[1] = boton;
//		llenarCasilla = 0;
//		casillaCompleta = 1;
//	}
}

void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}

void callback_extInt5(void){	//Monedas
	flagMoneda = 1;
//	dinero = dinero + 500;

}

// Función que se ejecuta cada vez que un caracter es recibido
void usart2Rx_Callback(void){
	usart2DataReceived = getRxData();

}

void BasicTimer5_Callback(void){
	conteoServo++;
}


void identificarF1(void){
	switch(banderaBarrido){
	case 1:{
		boton = 1;
		break;
	}
	case 2:{
		boton = 2;
		break;
	}
	case 3:{
		boton = 3;
		break;
	}
//	case 4:{
//		boton = 'A';
//		break;
//	}
	default:{
		break;
	}
	}
}

void identificarF2(void){
	switch(banderaBarrido){
	case 1:{
		boton = 4;
		break;
	}
	case 2:{
		boton = 5;
		break;
	}
	case 3:{
		boton = 6;
		break;
	}
//	case 4:{
//		boton = 'B';
//		break;
//	}
	default:{
		break;
	}
	}
}

void identificarF3(void){
	switch(banderaBarrido){
	case 1:{
		boton = 7;
		break;
	}
	case 2:{
		boton = 8;
		break;
	}
	case 3:{
		boton = 9;
		break;
	}
//	case 4:{
//		boton = 'C';
//		break;
//	}
	default:{
		break;
	}
	}
}

void identificarF4(void){
	switch(banderaBarrido){
	case 1:{
		boton = '*';
		break;
	}
	case 2:{
		boton = 0;
		break;
	}
	case 3:{
		boton = '#';
		break;
	}
//	case 4:{
//		boton = 'D';
//		break;
//	}
	default:{
		break;
	}
	}
}

void limpiar_casilla(void){
	for(uint8_t pos = 0; pos < 2; pos++){
		casilla[pos] = 0;
	}
}



