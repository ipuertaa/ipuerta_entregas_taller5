/*
 * RTC_Driver.c
 *
 *  Created on: Jun 7, 2023
 *      Author: isabel
 */

#include "RTC_Driver.h"

/*
 * 					INFORMACIÓN GENERAL
 *
 * - El LSE clock = Low Speed External crystal
 * Es un oscilador externo de 32.768 kHz. Que es la señal de reloj que controla al
 * RTC clock
 *
 * Es un reloj de tiempo real. Garantiza segundos, minutos, horas, dias, meses, años
 *
 *
 * 17.3.5 -> inicialización
 *
 * 1. Quitar la protección en el pwr control
 * 1. El pwr control también es un periférico entonces toca activar la señal de reloj
 * 2. Luego en power controller, en el cr, modificar el bit DBP para quitar la protección
 * 3. Activar lo relacionado al RTC. El RTC se activa en el RCC-BDCR-LSEON, SEÑAL ESTABLE
 * 4. Seleccionar la fuente de reloj en RTCSEL
 * 5. RCTEN en el mismo registro de arriba
 * 6. Ingresar claves en RTC_WPR con un =
 * 7. Activar el bypass de los shadow registers pag 439
 * en erading the calendar
 * 8. Poner modo inicialización ISR_INIT
 * revisar la bandera de modo inicialización
 * 9. Prescaler para que la salida sea de 1hertz, (no es necesario), por defecto ya quedan en 1hertz
 * 10. Configurar el formato: de 12 o 24 horas RTC_CR
 * 11. Luego configurar el time register
 * Función para dividir
 * 12. En el date register. }
 *
 *
 * Funciones para leer:
 *
 */


void configRTC (RTC_Handler_t *ptrRTC_Handler){

	/*
	 * El RTC tiene una protección de escritura. Para poder escribir sobre el RTC se debe quitar
	 * esa protección
	 */

	//1. Activar la señal de reloj del periférico power control
	RCC->CR |= RCC_APB1ENR_PWREN;

	//2. Deshabilitar la protección de escritura
	PWR->CR |= PWR_CR_DBP;

	//3. Habilitar el LSE
	RCC->BDCR |= RCC_BDCR_LSEON;

	//3.3 Esperar a que la señal de reloj LSE sea estable
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP();
	}

	//3.4 Seleccionar para que la fuente de reloj del RTC sea el LSE
	RCC->BDCR &= ~(RCC_BDCR_RTCSEL);
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;

	//3.5 Activar la señal de reloj para el RTC
	RCC->BDCR |= RCC_BDCR_RTCEN;

	/*
	 * 4. Para habilitar la escritura sobre los registros del RTC se deben
	 * escribir dos claves en el RTC_WPR
	 */
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	//Para escribir directamente sobre los registros del RTC, activamos el bypass
	RTC->CR |= RTC_CR_BYPSHAD;


}

