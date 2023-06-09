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
#include "PwmDriver.h"



// Elementos asociados al PLL
GPIO_Handler_t handlerMCO1 ={0};

// Elementos para la comunicación serial USART1
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;

// Elementos para el led de estado PA5 + TIM2
BasicTimer_Handler_t handlerStateOKTimer = {0};
GPIO_Handler_t handlerLedOK = {0};

// Elementos para el trabajo con el acelerómetro:

// Comunicación por I2C1
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};

// Barrido
BasicTimer_Handler_t handlerAcelerometro = {0};
uint16_t muestreo = 0;
uint16_t numDato = 0;

uint8_t FlagDatos = 0;

// Manejo de datos
float EjeX = 0;
float EjeY = 0;
float EjeZ = 0;
float dataX = 0;
float DatosEjeX[2000] = {0};
float DatosEjeY[2000] = {0};
float DatosEjeZ[2000] = {0};

//Elementos para el manejo de la pantalla

//Comunicación I2C2
GPIO_Handler_t handlerLCD_SDA = {0};
GPIO_Handler_t handlerLCD_SCL = {0};
I2C_Handler_t handlerLCD = {0};

// Elementos para el manejo de los PWM
GPIO_Handler_t handlerGPIO_PWM1		= {0};
PWM_Handler_t handlerPWM1			= {0};

GPIO_Handler_t handlerGPIO_PWM2		= {0};
PWM_Handler_t handlerPWM2			= {0};

GPIO_Handler_t handlerGPIO_PWM3		= {0};
PWM_Handler_t handlerPWM3			= {0};
uint16_t Dutty = 3277;
uint16_t NewDutty = 0;


// Elementos de uso general
char bufferData[100] = {0};
char bufferEjes[100] = {0};
uint8_t i2cBuffer = 0;
uint8_t captura = 0;
char mensajePrueba[100] = {0};
uint8_t dia = 24;
uint8_t mes = 5;
uint8_t anio = 23;
uint8_t banderaCaptura = 0;
uint16_t a = 0;
uint16_t frecuenciaPLL = 0;

char mensajeLCD[] = "Tarea Taller 5";
char hpta[]= "Eje x =";
char mensajeAccelX[] = "Eje X";
char mensajeAccelY[] = "Eje Y";
char mensajeAccelZ[] = "Eje Z";



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
float AccelZ_conv(void);

int16_t AccelX_non_conv(void);
int16_t AccelY_non_conv(void);
int16_t AccelZ_non_conv(void);


//Función principal del programa

int main(void){

	//Inicializar todos los elementos
	configPLL80MHz();
	//Imprimir un mensaje de inicio
	init_hardware();
	init_acelerometro();
	writeMsg(&handlerCommTerminal, "Tarea Especial Taller 5 \nIsabel Puerta Alvarez \n");
	writeMsg(&handlerCommTerminal, "Inicializando sistema, espere por favor\n");

//	Init_LCD(&handlerLCD);

	//Activar el punto flotante
	SCB->CPACR |= (0x0F << 20);

//	writeMsg(&handlerCommTerminal, mensajePrueba);


//	LCD_XY(&handlerLCD, 3, 2);
//	LCD_out_Msg(&handlerLCD, "Tarea Taller 5");
//	delay_ms(1000);
//	LCD_XY(&handlerLCD, 1, 4);
//	LCD_out_Msg(&handlerLCD, "Isabel Puerta A");
//	delay_ms(1000);
//	comandos_LCD(&handlerLCD, CLEAR);
//	delay_ms(10);
//
//	LCD_XY(&handlerLCD, 1, 1);
//	LCD_out_Msg(&handlerLCD, "Eje X =         m/s2");
//	LCD_XY(&handlerLCD, 1, 2);
//	LCD_out_Msg(&handlerLCD, "Eje Y =         m/s2");
//	LCD_XY(&handlerLCD, 1, 3);
//	LCD_out_Msg(&handlerLCD, "Eje Z =         m/s2");
//	LCD_XY(&handlerLCD, 1, 4);
//	LCD_out_Msg(&handlerLCD, "Sensit:16384 LSB/g");

	writeMsg(&handlerCommTerminal, "Sistema inicializado con éxito \nPulse 'o' para ver sus opciones\n");





	while(1){

		if(muestreo != 0){

			//Realizando el barrido de los datos cada 1KHz = 1 ms
			EjeZ = AccelX_conv();
			EjeY = AccelY_conv();
			EjeX = AccelZ_conv();

			muestreo = 0;
		}
		/*
		 * Se utiliza el la interrupción del timer asociado al led de estado para controlar que se refresquen
		 * los datos de aceleración cada 1s.  250 ms * 4 = 1000 ms = 1s
		 */
//		if(FlagDatos > 4){
//			sprintf(bufferData, "%.2f", EjeX);
//			LCD_XY(&handlerLCD, 9, 1);
//			LCD_out_Msg(&handlerLCD, bufferData);
//
//			sprintf(bufferData, "%.2f", EjeY);
//			LCD_XY(&handlerLCD, 9, 2);
//			LCD_out_Msg(&handlerLCD, bufferData);
//
//			sprintf(bufferData, "%.2f", EjeZ);
//			LCD_XY(&handlerLCD, 9, 3);
//			LCD_out_Msg(&handlerLCD, bufferData);
//			FlagDatos = 0;
//		}


		if(rxData != '\0'){
			if(rxData == 'o'){

				writeMsg(&handlerCommTerminal, "Oprimir 'x' para obtener el valor de la aceleración en el eje x\n");
				writeMsg(&handlerCommTerminal, "\nOprimir 'y' para obtener el valor de la aceleración en el eje y\n");
				writeMsg(&handlerCommTerminal, "\nOprimir 'z' para obtener el valor de la aceleración en el eje z\n");
				writeMsg(&handlerCommTerminal, "\nOprima la letra 'i' para obtener información sobre la frecuencia del MCU\n");
				writeMsg(&handlerCommTerminal, "\nOprima la letra 'c' y espere unos segundos para obtener una captura de 6000 datos de los 3 ejes.\nLa primera columna corresponderá a X, la segunda a Y y la tercera a Z\n");

				rxData = '\0';

			}


			else if(rxData == 'x'){

				sprintf(bufferData, "\nAccelX = %.2f m/s²\n", EjeX);
				writeMsg(&handlerCommTerminal, bufferData);

				rxData = '\0';
			}
			else if(rxData == 'y'){

				sprintf(bufferData, "\nAccelY = %.2f m/s² \n", EjeY);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){

				sprintf(bufferData, "\nAccelZ = %.2f m/s² \n", EjeZ);
 				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'i'){
				frecuenciaPLL = getConfigPLL(2, 80);
				sprintf(bufferData,"\nPLL frecuencia configurada = %u MHz \n ", frecuenciaPLL);
				writeMsg(&handlerCommTerminal, bufferData);

				writeMsg(&handlerCommTerminal, "Para la medición de la frecuencia se encuentra habilito el pin PA8,\n donde se podrá medir una frecuencia de aproximadamente 16MHz\n");
				rxData = '\0';
			}


			else if(rxData == 'c'){
				numDato = 0;
				sprintf(bufferData, "Se está realizando la captura de datos \n");
				writeMsg(&handlerCommTerminal, bufferData);

				while(numDato < 2000){ 		//Garantizar los 2 segundos de medición

					if(muestreo != 0){		//Garantizar que sólo tome datos cuando el timer lo indique

						EjeX = AccelX_conv();
						EjeY = AccelY_conv();
						EjeZ = AccelZ_conv();

						DatosEjeX[numDato] = EjeX;
						DatosEjeY[numDato] = EjeY;
						DatosEjeZ[numDato] = EjeZ;
						numDato++;
						muestreo = 0;
					}
				}

				//Al salir del ciclo anterior, quiere decir que ya se tomaron los datos y se procede a imprimirlos

				numDato = 0;
				sprintf(bufferData, "Dato X ; Dato Y ; Dato Z \n");
				writeMsg(&handlerCommTerminal, bufferData);
				delay_ms(500);

				while(numDato < 2000){
					sprintf(bufferData, "%.2f m/s² ; %.2f m/s² ; %.2f m/s² \n", DatosEjeX[numDato], DatosEjeY[numDato], DatosEjeZ[numDato]);
					writeMsg(&handlerCommTerminal, bufferData);
					numDato++;

				}

				sprintf(bufferData, "\nImpresion exitosa \n");
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else{
				rxData = '\0';
			}

		}	//Fin if(rxData != '0')

	}	//FIn while

}	//FIn main

int16_t AccelX_non_conv(void){
	uint8_t accelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
	uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_high << 8 | accelX_low;
	return AccelX;
}

int16_t AccelY_non_conv(void){
	uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;
	return AccelY;
}

int16_t AccelZ_non_conv(void){
	uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
	return AccelZ;
}




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


float AccelZ_conv(void){
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
	auxAccel = (valorAccel*9.81)/16384;
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

	handlerStateOKTimer.ptrTIMx  = TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us_80MHz;
	handlerStateOKTimer.TIMx_Config.TIMx_period = 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerStateOKTimer);

	//Configuración de la comunicación serial USART1

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




	// Comunicación I2C con el acelerómetro

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

	// Timer que controla el barrido del acelerómetro

	handlerAcelerometro.ptrTIMx  							= TIM5;
	handlerAcelerometro.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerAcelerometro.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_100us_80MHz;
	handlerAcelerometro.TIMx_Config.TIMx_period 			= 10;
	handlerAcelerometro.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerAcelerometro);


	// Comunicación I2C con la pantalla

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

	// Configuración del systick para que el reloj sea el PLL de 80MHz
	config_SysTick_ms(PLL_80MHz_CLOCK_CONFIGURED);



	// Configuración del pin para medir la frecuencia del micro

	handlerMCO1.pGPIOx 										= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	GPIO_Config(&handlerMCO1);

	//Configuración de los PWM

	//	PWM1
//	//Configuración del Pin para que funcione como salida de PWM
//
//	handlerGPIO_PWM1.pGPIOx									= GPIOC;
//	handlerGPIO_PWM1.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
//	handlerGPIO_PWM1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
//	handlerGPIO_PWM1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerGPIO_PWM1.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerGPIO_PWM1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
//	handlerGPIO_PWM1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
//
//	GPIO_Config(&handlerGPIO_PWM1);
//
//	//Configurar el timer para que genere la señal PWM
//
//	handlerPWM1.ptrTIMx							= TIM3;
//	handlerPWM1.config.channel					= PWM_CHANNEL_1;
//	handlerPWM1.config.periodo					= 600;
//	handlerPWM1.config.prescaler				= BTIMER_SPEED_100us_80MHz;
//	handlerPWM1.config.duttyCicle				= 300;
//
//	pwm_Config(&handlerPWM1);
//
//
//	//Activamos la señal de PWM por el TIM3
//	startPwmSignal(&handlerPWM1);
//	//Activamos la señal por el canal 1
//	enableOutput(&handlerPWM1);
//
//
//	//	PWM2
//	//Configuramos el pin para que funcione como salida de PWM
//
//	handlerGPIO_PWM2.pGPIOx									= GPIOC;
//	handlerGPIO_PWM2.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
//	handlerGPIO_PWM2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
//	handlerGPIO_PWM2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerGPIO_PWM2.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerGPIO_PWM2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
//	handlerGPIO_PWM2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
//
//	GPIO_Config(&handlerGPIO_PWM2);
//
//	//Configurar el timer para que genere la señal PWM
//
//	handlerPWM2.ptrTIMx							= TIM3;
//	handlerPWM2.config.channel					= PWM_CHANNEL_4;
//	handlerPWM2.config.periodo					= 300;
//	handlerPWM2.config.prescaler				= BTIMER_SPEED_100us_80MHz;
//	handlerPWM2.config.duttyCicle				= 150;
//
//	pwm_Config(&handlerPWM2);
//
//	//Activamos la señal de PWM por el TIM3
//	startPwmSignal(&handlerPWM2);
//	//Activamos la señal por el canal 4
//	enableOutput(&handlerPWM2);
//
//
//
//	//	PWM3
//	//Configuramos el pin para que funcione como salida de PWM
//
//	handlerGPIO_PWM3.pGPIOx									= GPIOC;
//	handlerGPIO_PWM3.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
//	handlerGPIO_PWM3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
//	handlerGPIO_PWM3.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerGPIO_PWM3.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerGPIO_PWM3.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
//	handlerGPIO_PWM3.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
//
//	GPIO_Config(&handlerGPIO_PWM3);
//
//	//Configurar el timer para que genere la señal PWM
//
//	handlerPWM3.ptrTIMx								= TIM3;
//	handlerPWM3.config.channel						= PWM_CHANNEL_3;
//	handlerPWM3.config.periodo						= 200;
//	handlerPWM3.config.prescaler					= BTIMER_SPEED_100us_80MHz;
//	handlerPWM3.config.duttyCicle					= 100;
//
//	pwm_Config(&handlerPWM3);
//
//	//Activamos la señal de PWM por el TIM3
//	startPwmSignal(&handlerPWM3);
//	//Activamos la señal por el canal 4
//	enableOutput(&handlerPWM3);

}	//Fin init_hardware






//Callback para la recepción del USART1

void usart1Rx_Callback(void){
	rxData = getRxData();
}

//Callback para la transmisión del USART1
void usart1Tx_Callback(void){

}


void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLedOK);
	FlagDatos++;
}

void BasicTimer5_Callback(void){
	muestreo++;


}

