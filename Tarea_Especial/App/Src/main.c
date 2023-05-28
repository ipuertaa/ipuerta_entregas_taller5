/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez

	Solución básica de u  proyecto con librerías externas
 ******************************************************************************
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
#include "LcdDriver.h"
#include "SysTickDriver.h"




GPIO_Handler_t handlerMCO1 ={0};

//Elementos para el usart
GPIO_Handler_t handlerLedOK = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
uint8_t rxData = 0;

BasicTimer_Handler_t handlerStateOKTimer = {0};
BasicTimer_Handler_t handlerAcelerometro = {0};
uint8_t muestreo = 0;
float EjeX = 0;
float EjeY = 0;
float EjeZ = 0;

USART_Handler_t handlerCommTerminal = {0};
char bufferData[64] = "Accel MPU-6050 testing";

uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint8_t systemTicksEnd = 0;

GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;



char mensajePrueba[100] = {0};
uint8_t dia = 24;
uint8_t mes = 5;
uint8_t anio = 23;

//Elementos para la comunicación con la pantalla
GPIO_Handler_t handlerLCD_SDA = {0};
GPIO_Handler_t handlerLCD_SCL = {0};
I2C_Handler_t handlerLCD = {0};

unsigned char mensajeLCD[] = "Tarea Especial TallerV";
unsigned char mensajeAccelX[] = "Accel X =";
unsigned char mensajeAccelY[] = "Accel Y =";
unsigned char mensajeAccelZ[] = "AccelZ =";

#define LCD_ADDRESS 	0x20;


#define ACCEL_ADDRESS	0b1101001;
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

//MIN 19

//Definición de funciones
void init_hardware(void);
float convertirUnidades(int16_t valorAccel);
void init_acelerometro(void);
float AccelX_conv(void);
float AccelY_conv(void);
float AccelZ_cov(void);

//Función principal del programa

int main(void){

	//Inicializar todos los elementos
	configPLL80MHz();
	init_hardware();

	//Imprimir un mensaje de inicio
	sprintf(mensajePrueba, "Iniciando sistema \n");
	writeMsg(&handlerCommTerminal, mensajePrueba);

	delay_ms(100);

	init_acelerometro();

	//Activar el punto flotante
	SCB->CPACR |= (0x0F << 20);

	Init_LCD(&handlerLCD);
	nuevoDato_LCD(&handlerLCD, 'i');
	nuevoDato_LCD(&handlerLCD, 's');
	nuevoDato_LCD(&handlerLCD, 'a');
	nuevoDato_LCD(&handlerLCD, 'b');
	nuevoDato_LCD(&handlerLCD, 'e');
	nuevoDato_LCD(&handlerLCD, 'l');

	delay_ms(1000);

	comandos_LCD(&handlerLCD, CLEAR);
	delay_ms(1000);

	LCD_out_Msg(&handlerLCD, mensajeLCD);
	delay_ms(10000);
















	while(1){

		if(muestreo != 0){
			EjeX = AccelX_conv();
			EjeY = AccelY_conv();
			EjeZ = AccelZ_cov();

			muestreo = 0;		//Para que sólo haga la toma de datos cada que el timer lo indique
		}


		if(rxData != '\0'){

			if(rxData == 'x'){

				uint8_t accelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | accelX_low;
				float AccelX_conv = convertirUnidades(AccelX);
				sprintf(bufferData, "AccelX = %#.2f m/s² \n", AccelX_conv);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){

				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				float AccelY_conv = convertirUnidades(AccelY);
				sprintf(bufferData, "AccelY = %#.2f m/s² \n", AccelY_conv);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				float AccelZ_conv = convertirUnidades(AccelZ);
				sprintf(bufferData, "AccelZ = %#.2f m/s² \n", AccelZ_conv);
 				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if(rxData == 's'){
				sprintf(bufferData, "AFS_SEL state (r)\n");
				writeMsg(&handlerCommTerminal, bufferData);
				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, AFS_SEL);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = 0;

			}
			else{
				rxData = '\0';
			}

		}



//		if(rxData != '\0'){
//			writeChar(&handlerCommTerminal, rxData);
//
//			if(rxData == 'w'){
//				sprintf(bufferData, "WHO_AM_I? (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
////
//				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
//				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//			}
//			else if(rxData == 'p'){
//
//				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//
//				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
//				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = 0;
//
//			}
//			else if(rxData == 'r'){
//				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//
//				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
//				rxData = '\0';
//			}
//
//			else if(rxData == 'x'){
//				sprintf(bufferData, "Axis X data (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//
//				uint8_t accelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
//				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
//				int16_t AccelX = AccelX_high << 8 | accelX_low;
//				sprintf(bufferData, "AccelX = %d \n", (int)AccelX);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//
//			}
//			else if(rxData == 'y'){
//				sprintf(bufferData, "Axis Y data (r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
//				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
//				int16_t AccelY = AccelY_high << 8 | AccelY_low;
//				sprintf(bufferData, "AccelY = %d \n", (int)AccelY);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//			}
//			else if(rxData == 'z'){
//				sprintf(bufferData, "Axis Z data(r)\n");
//				writeMsg(&handlerCommTerminal, bufferData);
//				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
//				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
//				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
//				sprintf(bufferData, "AccelZ = %d \n", (int)AccelZ);
//				writeMsg(&handlerCommTerminal, bufferData);
//				rxData = '\0';
//			}
//			else {
//				rxData = '\0';
//			}
//
//
//
//		}



	}	//Fin while




}	//Fin main

float AccelX_conv(void){
	uint8_t accelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
	uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_high << 8 | accelX_low;
	float AccelX_conv = convertirUnidades(AccelX);
	return AccelX_conv;

}
float AccelY_conv(void){
	uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;
	float AccelY_conv = convertirUnidades(AccelY);
	return AccelY_conv;
}


float AccelZ_cov(void){
	uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
	float AccelZ_conv = convertirUnidades(AccelZ);
	return AccelZ_conv;
}




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
	uint8_t SlaveAdress = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);

//	sprintf(bufferData, "WHO AM I = 0x%x \n", (unsigned int) i2cBuffer);
//	writeMsg(&handlerCommTerminal, bufferData);

	i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);

	uint8_t PWR_STATE = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);

//	sprintf(bufferData, "PWR_MGMT = 0x%x \n", (unsigned int) i2cBuffer);
//	writeMsg(&handlerCommTerminal, bufferData);

	if((SlaveAdress == 0x68) && (PWR_STATE == 0x00)){
		sprintf(bufferData, "Acelerometro inicializado \n");
		writeMsg(&handlerCommTerminal, bufferData);
	}
}

float convertirUnidades(int16_t valorAccel){
	float auxAccel = 0.0;
	auxAccel = (((float)valorAccel)/1638.4)* GRAVEDAD;
	return auxAccel;
}
void init_hardware(void){

	//configurar LED2 - PA5 - LED DE ESTADO

	handlerLedOK.pGPIOx 										= GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedOK);

	//Configuración de la comunicación serial:

	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	handlerCommTerminal.ptrUSARTx = USART1;
	handlerCommTerminal.USART_Config.USART_baudrate = USART_BAUDRATE_80MHz_115200;
	handlerCommTerminal.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX = USART_TX_INTERRUPT_ENABLE;

	USART_Config(&handlerCommTerminal);

	handlerStateOKTimer.ptrTIMx  = TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us_80MHz;
	handlerStateOKTimer.TIMx_Config.TIMx_period = 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerStateOKTimer);


	//Pines con los que funciona el I2C

	handlerI2cSCL.pGPIOx = GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;

	GPIO_Config(&handlerI2cSCL);

	handlerI2cSDA.pGPIOx	= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;

	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx = I2C1;
	handlerAccelerometer.modeI2C = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress = ACCEL_ADDRESS;
	handlerAccelerometer.Clock_Freq = CLOCK_FREQ_80MHz;

	i2c_config(&handlerAccelerometer);


//	//Configurar la comunicación con la pantalla
	handlerLCD_SCL.pGPIOx		= GPIOB;
	handlerLCD_SCL.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	handlerLCD_SCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerLCD_SCL.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	handlerLCD_SCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerLCD_SCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLCD_SCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;

	GPIO_Config(&handlerLCD_SCL);

	handlerLCD_SDA.pGPIOx		= GPIOB;
	handlerLCD_SDA.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerLCD_SDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerLCD_SDA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	handlerLCD_SDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerLCD_SDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLCD_SDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF9;

	GPIO_Config(&handlerLCD_SDA);

	handlerLCD.ptrI2Cx = I2C2;
	handlerLCD.modeI2C = I2C_MODE_SM;
	handlerLCD.slaveAddress = LCD_ADDRESS;
	handlerLCD.Clock_Freq = CLOCK_FREQ_80MHz;

	i2c_config(&handlerLCD);

	//Configuro el systick para que el reloj sea el PLL de 80MHz
	config_SysTick_ms(PLL_80MHz_CLOCK_CONFIGURED);



	//configurar el pin para medir la señal de reloj del micro

	handlerMCO1.pGPIOx 										= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	GPIO_Config(&handlerMCO1);

	handlerAcelerometro.ptrTIMx  = TIM3;
	handlerAcelerometro.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerAcelerometro.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us_80MHz;
	handlerAcelerometro.TIMx_Config.TIMx_period = 10;
	handlerAcelerometro.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerAcelerometro);




}





//Callback para la recepción del USART2

void usart1Rx_Callback(void){
	rxData = getRxData();
}

//Callback para la transmisión del USART2
void usart2Tx_Callback(void){

}


void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLedOK);
}

void BasicTimer3_Callback(void){
	muestreo++;
}

