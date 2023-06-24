

#include <stm32f4xx.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"

// Definición de los handlers necesarios

GPIO_Handler_t handlerBlinkyLED			= {0};
GPIO_Handler_t handlerUserButton		= {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

//Elementos para la comunicación serial
USART_Handler_t handlerUsart2			= {0};
GPIO_Handler_t handlerPinTX 			= {0};
GPIO_Handler_t handlerPinRX 			= {0};
uint8_t usart2DataReceived = 0;

//Elementos para el PWM
GPIO_Handler_t handlerMotor1			= {0};
PWM_Handler_t handlerPWM_Motor1			= {0};

GPIO_Handler_t handlerMotor2			= {0};
PWM_Handler_t handlerPWM_Motor2			= {0};

GPIO_Handler_t handlerMotor3			= {0};
PWM_Handler_t handlerPWM_Motor3			= {0};

GPIO_Handler_t handlerMotor4			= {0};
PWM_Handler_t handlerPWM_Motor4			= {0};

#define DUTTY 		2000;
uint8_t motorActivo = 0;

char bufferMsg[64] = {0};




//Definición de funciones
void init_hardware(void);


//Función principal del programa

int main(void){

	//Inicializar todos los elementos
	init_hardware();
	startPwmSignal(&handlerPWM_Motor1);
	disableOutput(&handlerPWM_Motor1);
	disableOutput(&handlerPWM_Motor2);
	disableOutput(&handlerPWM_Motor3);
	disableOutput(&handlerPWM_Motor4);
/*
* Con startPwmSignal se activa todo el timer configurado. Lo ideal es que todos los motores
* estén en el mismo timer
*
*/


	while(1){

		if(usart2DataReceived != '\0'){
			if(usart2DataReceived == 'A'){

				//Apagamos los otros canales
				disableOutput(&handlerPWM_Motor2);
				disableOutput(&handlerPWM_Motor3);
				disableOutput(&handlerPWM_Motor4);
				//Activamos la señal del motor 1
				enableOutput(&handlerPWM_Motor1);

				motorActivo = 1;

			}

			if(usart2DataReceived == 'B'){

				//Apagamos los otros canales
				disableOutput(&handlerPWM_Motor1);
				disableOutput(&handlerPWM_Motor3);
				disableOutput(&handlerPWM_Motor4);

				//Activamos la señal del motor 2
				enableOutput(&handlerPWM_Motor2);

				motorActivo = 2;

			}
			if(usart2DataReceived == 'C'){

				//Apagamos los otros canales
				disableOutput(&handlerPWM_Motor1);
				disableOutput(&handlerPWM_Motor2);
				disableOutput(&handlerPWM_Motor4);

				//Activamos la señal del motor 3
				enableOutput(&handlerPWM_Motor3);


				motorActivo = 3;
			}
			if(usart2DataReceived == 'D'){

				//Apagamos los otros canales
				disableOutput(&handlerPWM_Motor1);
				disableOutput(&handlerPWM_Motor2);
				disableOutput(&handlerPWM_Motor3);

				//Activamos la señal del motor 4
				enableOutput(&handlerPWM_Motor4);

				motorActivo = 4;

			}

			//Imprimir mensaje

			sprintf(bufferMsg, "Motor activo = %u \n", (unsigned int)motorActivo);
			writeMsg(&handlerUsart2, bufferMsg);

			//Cambiamos el estado del elemento que controla la entrada
			usart2DataReceived = '\0';

		}
	}	//Fin while


}	//Fin main

void init_hardware(void){

	//configurar LED2 - PA5 - LED DE ESTADO

	handlerBlinkyLED.pGPIOx 										= GPIOA;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLED.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerBlinkyLED);

	//Configurar el timer para hacer el blinky del led de estado. Lanza interrupción cada 250 ms

	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);

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

	handlerUsart2.ptrUSARTx = USART2;
	handlerUsart2.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	handlerUsart2.USART_Config.USART_enableIntTX = USART_TX_INTERRUPT_DISABLE;

	USART_Config(&handlerUsart2);

	// CONFIGURACIÓN DE LOS MOTORES -> PWM

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

	//Activamos la señal del motor 1
	enableOutput(&handlerPWM_Motor1);
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
	enableOutput(&handlerPWM_Motor2);
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
	enableOutput(&handlerPWM_Motor3);
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
	//Activamos la señal del motor 4
	enableOutput(&handlerPWM_Motor4);
//	startPwmSignal(&handlerPWM_Motor4);

}





//Timer para el LED de estado
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerBlinkyLED);
}

// Función que se ejecuta cada vez que un caracter es recibido
void usart2Rx_Callback(void){
	usart2DataReceived = getRxData();

}

