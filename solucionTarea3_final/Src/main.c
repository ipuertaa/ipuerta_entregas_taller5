/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Isabel Puerta Alvarez
 * Created on: 03/03/23
 ******************************************************************************

 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


/*  DEFINICIÓN DE HANDLERS Y VARIABLES  */

GPIO_Handler_t handlerTransistor1 			= {0};
GPIO_Handler_t handlerTransistor2 			= {0};
GPIO_Handler_t handlerLedA 					= {0};
GPIO_Handler_t handlerLedB 					= {0};
GPIO_Handler_t handlerLedC 					= {0};
GPIO_Handler_t handlerLedD 					= {0};
GPIO_Handler_t handlerLedE 					= {0};
GPIO_Handler_t handlerLedF 					= {0};
GPIO_Handler_t handlerLedG 					= {0};
GPIO_Handler_t handlerLED2 					= {0};			//Led de estado

GPIO_Handler_t handlerButton 				= {0};			//Botón del encoder
GPIO_Handler_t handlerEncoder 				= {0};			//Data del encoder
GPIO_Handler_t handlerClock 				= {0};			//Clock del encoder

BasicTimer_Handler_t handlerBlinkyTimer 	= {0};			//Timer para el blinky del led de estado
BasicTimer_Handler_t handlerTransistores 	= {0};			//Timer para el barrido de los transistores


EXTI_Config_t handlerExtiEncoder 			= {0};			//EXTI para el data del encoder
EXTI_Config_t handlerExtiButton 			= {0};			//EXTI para el botón del encoder


//Las siguientes son variables auxiliares para el desarollo del código.

uint8_t modo 				= 0;
uint8_t banderaEntrada 		= 0;
uint8_t banderaBarrido 		= 0;
int8_t  conteoCulebrita 	= 0;
uint8_t auxEncoder 			= 0;
int64_t auxConteo 			= 0;
uint8_t auxClock 			= 0;
uint8_t unidades 			= 0;
uint8_t decenas 			= 0;
uint8_t entradaCulebrita 	= 0;


/*  CABECERAS DE FUNCIONES  */

void init_hardware(void);
void comparacion(void);
void variacion(uint8_t tipoUni_Dec);
void mostrar(uint8_t tipoUni_Dec);
void display (uint8_t tipoUni_Dec, char  segmento);
void culebrita(void);
void clearLeds(void);
void num0(void);
void num1(void);
void num2(void);
void num3(void);
void num4(void);
void num5(void);
void num6(void);
void num7(void);
void num8(void);
void num9(void);


/*  DESARROLLO DEL CÓDIGO  */

int main(void){

	//Con la función init_hardware se configuran todos los GPIO, TIMERS y EXTI a utilizar.

	init_hardware();

	while(1){


		switch (modo){

		//modo = 0, corresponde al modo conteo
		case 0:{

			if (banderaEntrada == 0){

				switch (banderaBarrido){

				case 1: {					//Encender decenas

					GPIO_WritePin(&handlerTransistor1, SET);
					GPIO_WritePin(&handlerTransistor2, RESET);

					variacion(1);

					banderaEntrada = 1;		//Para que no entre infinitas veces en el while
					break;
				}

				case 2:{					//Encender unidades

					GPIO_WritePin(&handlerTransistor1, RESET);
					GPIO_WritePin(&handlerTransistor2, SET);

					variacion(2);

					banderaBarrido = 0;		//Para que se reinicie el barrio
					banderaEntrada = 1;
					break;

				}
				default:{
					__NOP();
					break;
				}
				}
			}	//Fin if bandera entrada

			else {
				__NOP();
			}
		}	//Fin modo conteo

		//modo = 1, corresponde al modo culebrita
		case 1:{

			if (entradaCulebrita == 0){
				culebrita();
				entradaCulebrita = 1;
			}

			else{
				__NOP();
			}
		}	// Fin modo culebrita
	}	//Fin modo

} 	// FIN WHILE
}	// FIN MAIN


/*	DESARROLLO DE LAS FUNCIONES	 */

// Mediante la función init_hardware se cargan las configuraciones de todos los periféricos a utilizar

void init_hardware(void){

	//configurar LED2 - PA5 - LED DE ESTADO

	handlerLED2.pGPIOx 										= GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLED2);

	//Configurar el timer para hacer el blinky del led de estado. Lanza interrupción cada 250 ms

	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);

	//Configuración del pin que controla el transistor 1 -> segmento derecho.

	handlerTransistor1.pGPIOx								= GPIOC;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerTransistor1);

	//Configuración del pin que controla el transistor 2 -> segmento izquierdo

	handlerTransistor2.pGPIOx								= GPIOC;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinNumber		= PIN_11;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerTransistor2);

	//Configuración del la posición A del 7 segmentos

	handlerLedA.pGPIOx										= GPIOC;
	handlerLedA.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerLedA.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerLedA.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedA.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedA.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedA);

	//Configuración del la posición B del 7 segmentos

	handlerLedB.pGPIOx										= GPIOC;
	handlerLedB.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerLedB.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerLedB.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedB.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedB.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedB);

	//Configuración del la posición C del 7 segmentos

	handlerLedC.pGPIOx										= GPIOA;
	handlerLedC.GPIO_PinConfig.GPIO_PinNumber				= PIN_11;
	handlerLedC.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerLedC.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedC.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedC.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedC);

	//Configuración del la posición D del 7 segmentos

	handlerLedD.pGPIOx										= GPIOB;
	handlerLedD.GPIO_PinConfig.GPIO_PinNumber				= PIN_12;
	handlerLedD.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerLedD.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedD.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedD.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedD);

	//Configuración del la posición E del 7 segmentos

	handlerLedE.pGPIOx										= GPIOB;
	handlerLedE.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerLedE.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerLedE.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedE.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedE.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedE);

	//Configuración del la posición F del 7 segmentos

	handlerLedF.pGPIOx										= GPIOC;
	handlerLedF.GPIO_PinConfig.GPIO_PinNumber				= PIN_12;
	handlerLedF.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerLedF.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedF.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedF.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedF);

	//Configuración del la posición G del 7 segmentos

	handlerLedG.pGPIOx										= GPIOB;
	handlerLedG.GPIO_PinConfig.GPIO_PinNumber				= PIN_15;
	handlerLedG.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerLedG.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerLedG.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerLedG.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerLedG);

	// Configuración del timer para controlar los transistores. Van a intercalarse cada 10 ms.

	handlerTransistores.ptrTIMx 							= TIM3;
	handlerTransistores.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerTransistores.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerTransistores.TIMx_Config.TIMx_period 			= 10;
	handlerTransistores.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerTransistores);


	/*  CONFIGURACIÓN DE LAS INTERRUPCIONES
	 * Para configurar una interrupción pimero debo cargar en el handler la configuración del GPIO
	 * correspondiente como entrada digital, posteriormente, se carga la configuración del EXTI
	 * de ese número de pin, el cual, recibirá la posición de memoria de la
	 * configuración del GPIO (para saber de cuál pin viene la interrupción) y también se configura si
	 * la interrupción se dará cada flanco de subida o de bajada.
	 *
	 * Tanto el dato, clock y el botón del encoder están usualmente en alto, y al generar la interrupción
	 * se pasa a un cero lógico (es decir, flancos de bajada)
	 *
	 * Como utilicé el circuito integrado Schmitt trigger inversor SN74HC14N, configuré las interrupciones
	 * para que se dieran cada flanco de subida.
	 *
	 *
	 * Un Schmitt trigger se utiliza para evitar errores causados por señales ruidosas.
	 *
	 * En escencia es un amplificador operacional en modo comparación, donde no se tiene uno, sino dos
	 * umbrales o valores de referencia (uno alto y uno bajo), y comparando estos es que se obtendrá
	 * la señal a la salida. Si está por encima del corte alto, a la salida se tendrá un 1 lógico
	 * y si esta por debajo del corte bajo, se tendrá un cero lógico. De esta forma, se ignoran picos
	 * intermedios que el sistema puede interpretar como falsos pulsos.
	 *
	 * El encoder, al ser un elemento con componentes mecánicos, al cambiar de estado (por ejemplo al
	 * presionar el botón o mover la perilla) no se genera una señal perfectamente cuadrada, sino que
	 * hay pequeños picos que el microcontrolador puede entender como interrupciones y causar efectos
	 * indeseados.
	 *
	 * Estas perturbaciones son de alta frecuencia, por lo que, con un filtro pasa bajas, se podría
	 * corregir el error. En mi caso, el filtro no fue suficiente. En especial en el modo culebrita no se
	 * apreciaba bien el efecto porque el sistema se veía muy afectado por el ruido. Por ejemplo un valor
	 * que debía pasar a 1 cuando se girara la perilla, saltaba hasta 3 o 4.
	 *
	 * Por lo cual decidí utilizar tanto el filtro como el circuito integrado para poder tener cambios
	 * muy precisos.
	 * */

	//Configuración del GPIO del pin data del encoder

	handlerEncoder.pGPIOx 									= GPIOB;
	handlerEncoder.GPIO_PinConfig.GPIO_PinNumber 			= PIN_8;
	handlerEncoder.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_IN;
	handlerEncoder.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	//Configuración del EXTI del pin data del encoder

	handlerExtiEncoder.pGPIOHandler 						= &handlerEncoder;
	handlerExtiEncoder.edgeType 							= EXTERNAL_INTERRUPT_RISING_EDGE;

	extInt_Config(&handlerExtiEncoder);

	//Configuración del GPIO del botón del encoder

	handlerButton.pGPIOx									= GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerButton.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_IN;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	//Configuración del EXTI del boton del encoder

	handlerExtiButton.pGPIOHandler 							= &handlerButton;
	handlerExtiButton.edgeType 								= EXTERNAL_INTERRUPT_RISING_EDGE;

	extInt_Config(&handlerExtiButton);

	//Configuración del GPIO para la señal de reloj del encoder. No es necesario configurar EXTI

	handlerClock.pGPIOx 									= GPIOB;
	handlerClock.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
	handlerClock.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_IN;
	handlerClock.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerClock);

}	//Fin init_hardware


/* Mediante las funciones numX se escribe sobre los pines un estado alto (1) para apagarlo y un
 * estado bajo (0) para encenderlo. Esto es porque el 7 segmentos utilizado es de ánodo (positivo) común,
 * por lo que, si quiero encender el led, necesito escribir un cero, para que se pueda cerrar el circuito */

void num0(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, SET);
}

void num1(void){
	GPIO_WritePin(&handlerLedA, SET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, SET);
}

void num2(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, SET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num3(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num4(void){
	GPIO_WritePin(&handlerLedA, SET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num5(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, SET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num6(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, SET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num7(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, SET);
	GPIO_WritePin(&handlerLedG, SET);
}

void num8(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, RESET);
	GPIO_WritePin(&handlerLedE, RESET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

void num9(void){
	GPIO_WritePin(&handlerLedA, RESET);
	GPIO_WritePin(&handlerLedB, RESET);
	GPIO_WritePin(&handlerLedC, RESET);
	GPIO_WritePin(&handlerLedD, SET);
	GPIO_WritePin(&handlerLedE, SET);
	GPIO_WritePin(&handlerLedF, RESET);
	GPIO_WritePin(&handlerLedG, RESET);
}

/* Para la solución del problema, se consideró por separado las decenas y las unidades, la función
 * mostrar, que recibe como parámetro las unidades o las decenas muestra el valor correspondiente llamando
 * a la función numX
 *
 * Al recibir el parámetro tipoUni_Dec, se puede utilizar para que la función muestre el valor de unidades
 * o decenas según sea necesario. Esto es clave porque, si por ejemplo necesito mostrar el número 13,
 * debe aparecer un 1 en el segmento correspondiente a las unidades y un 3 en el correspondiente a las decenas
 * */

void mostrar(uint8_t tipoUni_Dec){
	switch (tipoUni_Dec){

	case 0: {
		num0();
		break;
	}
	case 1: {
		num1();
		break;
	}
	case 2: {
		num2();
		break;
	}
	case 3: {
		num3();
		break;
	}
	case 4: {
		num4();
		break;
	}
	case 5: {
		num5();
		break;
	}
	case 6: {
		num6();
		break;
	}
	case 7: {
		num7();
		break;
	}
	case 8: {
		num8();
		break;
	}
	case 9: {
		num9();
		break;
	}
	default:{
		__NOP();
		break;
	}
	} 		//Fin switch tipo
}	//Fin mostrar


/* Solución ejercicio #4
 * La función variación es clave para llevar a cabo el modo conteo.
 *
 * Cada vez que se gira el encoder, el conteo se va acumulando en la variable auxConteo (si el conteo
 * aumenta o disminuye se verá en una función más adelante).
 *
 * Dependiendo del valor de la variable auxConteo, se identifica cuál debe ser el valor de unidades
 * y decenas. Por ejemplo para el número 15, sería un 1 en las decenas y un 5 en las unidades.
 *
 * Posteriormente, con un condicional if else se evalúa, si se desea mostrar el valor correspondiente
 * a las decenas (tipoUni_Dec = 1) o a las unidades (tipoUni_Dec = 2).
 * Finalmente, se llama a la función mostrar, con el tipo correspondiente.
 *
 * Con el primer if, se garantiza que el contador no baje de cero y con el último else if se garantiza
 * que el contador no aumente más de 99*/

void variacion(uint8_t tipoUni_Dec){

	if (auxConteo <= 0){		//Límite inferior
		auxConteo = 0;
		unidades = 0;
		decenas = 0;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo > 0) && (auxConteo < 10)){
		unidades = auxConteo;
		decenas = 0;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 10) || (auxConteo < 20)){
		unidades = auxConteo - 10;
		decenas = 1;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 20) || (auxConteo < 30)){
		decenas = 2;
		unidades = auxConteo - 20;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 30) || (auxConteo < 40)){
		unidades = auxConteo - 30;
		decenas = 3;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 40) || (auxConteo < 50)){
		unidades = auxConteo - 40;
		decenas = 4;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 50) || (auxConteo < 60)){
		unidades = auxConteo - 50;
		decenas = 5;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 60) || (auxConteo < 70)){
		unidades = auxConteo - 60;
		decenas = 6;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 70) || (auxConteo < 80)){
		unidades = auxConteo - 70;
		decenas = 7;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 80) || (auxConteo < 90)){
		unidades = auxConteo - 80;
		decenas = 8;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if ((auxConteo == 90) || (auxConteo < 100)){
		unidades = auxConteo - 90;
		decenas = 9;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}

	else if(auxConteo >= 100){		//Límite superior
		auxConteo = 99;
		unidades = 9;
		decenas = 9;

		if (tipoUni_Dec == 1){
			mostrar(decenas);
		}
		else if(tipoUni_Dec == 2){
			mostrar(unidades);
		}
		else{
			__NOP();
		}
	}
	else {
		__NOP();
	}
}	//Fin variacion




/*	La función display es utilizada en el modo culebrita. Esta función recibe el tipoUni_Dec, en este caso,
 * tipoUni_Dec = 1, corresponde a mostrar las unidades y tipoUni_Dec = 2, corresponde a mostrar las decenas.
 * Con las funciones GPIO_WritePin, se configuran los transistores para que se encienda y apague el
 * correspondiente.
 *
 * En segmento, se indica con letras, cuál segmento es el que se quiere encender, por ejemplo, el segmento
 * 'g' es el del medio.
 *
 * Al combinar estos dos parámetros se puede encender de forma controlada, cada segmento del display.*/

void display (uint8_t tipoUni_Dec, char  segmento){

	if (tipoUni_Dec == 1){
		GPIO_WritePin(&handlerTransistor1, RESET);
		GPIO_WritePin(&handlerTransistor2, SET);
	}
	else if (tipoUni_Dec == 2){
		GPIO_WritePin(&handlerTransistor1, SET);
		GPIO_WritePin(&handlerTransistor2, RESET);
	}
	else {
		__NOP();
	}

	switch (segmento){

	case 'A':{
		GPIO_WritePin(&handlerLedA, RESET);
		GPIO_WritePin(&handlerLedB, SET);
		GPIO_WritePin(&handlerLedC, SET);
		GPIO_WritePin(&handlerLedD, SET);
		GPIO_WritePin(&handlerLedE, SET);
		GPIO_WritePin(&handlerLedF, SET);
		GPIO_WritePin(&handlerLedG, SET);
		break;
	}
	case 'B':{
		GPIO_WritePin(&handlerLedA, SET);
		GPIO_WritePin(&handlerLedB, RESET);
		GPIO_WritePin(&handlerLedC, SET);
		GPIO_WritePin(&handlerLedD, SET);
		GPIO_WritePin(&handlerLedE, SET);
		GPIO_WritePin(&handlerLedF, SET);
		GPIO_WritePin(&handlerLedG, SET);
		break;
	}
	case 'C':{
		GPIO_WritePin(&handlerLedA, SET);
		GPIO_WritePin(&handlerLedB, SET);
		GPIO_WritePin(&handlerLedC, RESET);
		GPIO_WritePin(&handlerLedD, SET);
		GPIO_WritePin(&handlerLedE, SET);
		GPIO_WritePin(&handlerLedF, SET);
		GPIO_WritePin(&handlerLedG, SET);
		break;
	}
	case 'D':{
		GPIO_WritePin(&handlerLedA, SET);
		GPIO_WritePin(&handlerLedB, SET);
		GPIO_WritePin(&handlerLedC, SET);
		GPIO_WritePin(&handlerLedD, RESET);
		GPIO_WritePin(&handlerLedE, SET);
		GPIO_WritePin(&handlerLedF, SET);
		GPIO_WritePin(&handlerLedG, SET);
		break;
	}
	case 'E':{
		GPIO_WritePin(&handlerLedA, SET);
		GPIO_WritePin(&handlerLedB, SET);
		GPIO_WritePin(&handlerLedC, SET);
		GPIO_WritePin(&handlerLedD, SET);
		GPIO_WritePin(&handlerLedE, RESET);
		GPIO_WritePin(&handlerLedF, SET);
		GPIO_WritePin(&handlerLedG, SET);
		break;
	}
	case 'F':{
		GPIO_WritePin(&handlerLedA, SET);
		GPIO_WritePin(&handlerLedB, SET);
		GPIO_WritePin(&handlerLedC, SET);
		GPIO_WritePin(&handlerLedD, SET);
		GPIO_WritePin(&handlerLedE, SET);
		GPIO_WritePin(&handlerLedF, RESET);
		GPIO_WritePin(&handlerLedG, SET);
	}
	default:{
		__NOP();
		break;
	}
}
}	//Fin display

/*	Solución al ejercicio #6
 * La función culebrita, es la que indica la secuencia a llevar a cabo dependiendo del conteo que se esté
 *  llevando.
 *
 *  De nuevo, más adelante se muestra cómo se hizo la diferenciación entre sentido horario y antiohorario de
 *  giro del encoder.
 *
 *  Cada vez que gira el encoder se va almacenando el conteo en la variable conteoCulebria (debe ser una
 *  variable de conteo diferente a la del modo mostrar números para que se pueda guardar el estado de los dos
 *  modos de forma independiente)
 *
 *  La función culebrita, toma este valor de conteo y lo evalúa:
 *  - Verifica sus posiciones extremas, para que el conteo siempre esté entre los valores deseados, es
 *  decir, la culebrita sea cíclica.
 *  - Dependiendo de ese valor de conteo, muestra el segmento de LED particular, ya sea en el lado de las
 *  decenas o de las unidades. Esto siguiendo la secuencia que fue indicada.
 *
 */
void culebrita(void){

	if(conteoCulebrita < 0){		//Tope de la culebrita CCW
		conteoCulebrita = 11;
	}

	switch (conteoCulebrita){

	case 0:{
		display(1,'A');
		break;
	}
	case 1:{
		display(2,'A');
		break;
	}
	case 2:{
		display(2,'F');
		break;
	}
	case 3:{
		display(2,'E');
		break;
	}
	case 4:{
		display(2,'D');
		break;
	}
	case 5:{
		display(1,'E');
		break;
	}
	case 6:{
		display(1,'F');
		break;
	}
	case 7:{
		display(2,'B');
		break;
	}
	case 8:{
		display(2,'C');
		break;
	}
	case 9:{
		display(1,'D');
		break;
	}
	case 10:{
		display(1, 'C');
		break;
	}
	case 11:{
		display(1, 'B');
		break;
	}
	case 12:{
		display(1, 'A');
		break;
	}
	default:{
		__NOP();
		break;
	}
	}	//Fin switch

	if(conteoCulebrita >= 12){
		conteoCulebrita = 0;			//Tope de la culebrita CW
	}
}	//Fin culebrita



/* La función comparación junto con el callback_extInt8 es la solución al ejercicio #3 de la tarea.
 *
 * Esta función compara el valor de auxEncoder, correspondiente al pin dato del encoder y el valor
 * auxClock que corresponde al pin clock del encoder.
 *
 * Desde el callback, cada vez que sucede la interrupción dada por el giro del encoder, la variable
 * auxEncoder se hace igual a 1, y la variable auxClock guarda el estado del reloj en ese instante.
 *
 * Al visualizar la señales en el pulseView, se pudo apreciar que:
 * - Sentido CW: cuando se da el flanco de subida en el pin dato, el reloj está en estado alto.
 * - Sentido CCW: cuando se da el flanco de subida en el pin dato, el reloj está en estado bajo.
 *
 * Con esto, se puede saber la dirección de giro, dependiendo entonces, si es CW o CCW, se suma
 * o resta a las variables que almacenan el conteo.
 * Incrementar o decrementar hace parte de la solución al ejercicio #4
 *
 * Para que cada conteo (el de la culebrita y el de los números) sea independiente, se evalúa cuál modo
 * está activo (modo 0 para el conteo o modo 1 para la culebrita) y se suma la variable correspondiente.
 *
 */
void comparacion(void){

	if (auxEncoder == auxClock){	//CW

		if (modo == 0){
			auxConteo++;
		}
		else if (modo == 1){
			conteoCulebrita++;
		}
		else{
			__NOP();
		}

	}
	else if (auxEncoder != auxClock){	//CCW

		if (modo == 0){
			auxConteo--;
		}
		else if (modo == 1){
			conteoCulebrita--;
		}
		else{
			__NOP();
		}
	}
	else{
		__NOP();
	}

	auxEncoder = 0;
}	//FIN COMPARACION


/* Solución al ejercicio #2 -> Implementación del EXTI  */
void callback_extInt8(void){						//Callback del encoder
	auxEncoder = 1;
	auxClock = GPIO_ReadPin(&handlerClock);
	comparacion();
	entradaCulebrita = 0;
}

/* Solución al ejercicio #5.
 * Con el callback del botón se puede identificar en qué modo se está trabajando.
 * Si no se ha apretado el botón, será el modo 0, correspondiente al conteo
 * Si se aprieta una vez, será el modo 1, correspondiente a la culebrita
 * Si se aprieta otra vez, será nuevamente el modo cero */
void callback_extInt9(void){						//Callback del boton
	modo ^= 1;
	GPIO_WritePin(&handlerTransistor1, SET);
	GPIO_WritePin(&handlerTransistor2, SET);
	entradaCulebrita = 0;
}


/* Solución al ejercicio #1 -> Implementación del led de estado  */
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLED2);
}

void BasicTimer3_Callback(void){
	banderaBarrido++;
	banderaEntrada = 0;
}


/* las variables entradaCulebrita y banderaEntrada, son utilizadas para hacer un barrido controlado.
 * Como el código se lleva a cabo dentro del while infinito, no es deseable que se esté repitiendo todo
 * el proceso indefinidamente, sino que sólo se lleve a cabo cada vez que sea necesario. Así que con estas,
 * se garantiza la entrada y la salida.
 *
 *
 * Por ejemplo, los transistores tienen que hacer una especie de barrido (se deben intercalar) cada que el
 * timer3 dé la orden.
 * banderaBarrido con el valor 1 o 2, indica cuál transistor es el que se va a encender (esto se hace en
 * el main) y con bandera entrada garantizo que, dentro de un mismo ciclo del timer no se esté repitiendo
 * el encendido y apagado de los leds.
 *
 * La función entradaCulebrita también garantiza que la culebrita se actualice sólo cuando la interrupción
 * externa lo indique
 *
 */



