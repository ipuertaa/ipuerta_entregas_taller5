/*
 * RTC_Driver.c
 *
 *  Created on: Jun 7, 2023
 *      Author: isabel
 */


#include <RTC_Driver.h>
#include "stm32f4xx.h"

uint8_t decenas = 0;
uint8_t unidades = 0;

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
 *
 */


void configRTC (RTC_Handler_t *ptrRTC_Handler){

	/*
	 * El RTC tiene una protección de escritura. Para poder escribir sobre el RTC se debe quitar
	 * esa protección
	 */

	//1. Activar la señal de reloj del periférico power control
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

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

	//5. Para escribir directamente sobre los registros del RTC, activamos el bypass
	RTC->CR |= RTC_CR_BYPSHAD;

	//Inicialización para programar el calendario
	//6. Activar el modo inicialización. Se detiene el contador del calendario para que se pueda modificar
	RTC->ISR |= RTC_ISR_INIT;

	//6.1 Esperar a que se levante la bandera de la inicialización
	while(!(RTC->ISR & RTC_ISR_INITF)){
		__NOP();
	}

	//6.2 Configurar el prescaler para garantizar una frecuencia de 1Hz para el contador del calendario
	//El manual indica que se debe cargar primero el prescaler síncrono y luego el asíncrono

	RTC->PRER = 0;

	RTC->PRER |= (0xFF << RTC_PRER_PREDIV_S_Pos);
	RTC->PRER |= (0x7F << RTC_PRER_PREDIV_A_Pos);

	//6.3 Configurar el formato de la hora y si es AM o PM
	if(ptrRTC_Handler->formatoHora == FORMATO_12HORAS){
		RTC->CR |= RTC_CR_FMT;

		if(ptrRTC_Handler->Am_pm == AM){
			RTC->TR &= ~RTC_TR_PM;
		}
		else if(ptrRTC_Handler->Am_pm == PM){
			RTC->TR |= RTC_TR_PM;
		}
		else{
			__NOP();
		}

	}
	else if(ptrRTC_Handler->formatoHora == FORMATO_24HORAS){
		RTC->CR &= ~RTC_CR_FMT;
		RTC->TR &= ~RTC_TR_PM;

	}
	else{
		__NOP();
	}

	//6.3 CONFIGURAR EL CALENDARIO. En el DR- dia- mes- diaSemana - año
	RTC->DR = 0;

	//6.3.1 Dividir el día en unidades y decenas
	dividirValor(ptrRTC_Handler->dia);

	//Cargar las unidades del día
	RTC->DR |= (unidades << RTC_DR_DU_Pos);

	//Cargar las decenas del día
	RTC->DR |= (decenas << RTC_DR_DT_Pos);

	//6.3.2 Dividir el mes en unidades y decenas
	dividirValor(ptrRTC_Handler->mes);

	//Cargar las unidades del mes
	RTC->DR |= (unidades << RTC_DR_MU_Pos);

	//Cargar las decenas del mes
	RTC->DR |= (decenas << RTC_DR_MT_Pos);

	//6.3.3 Configurar el día de la semana
	RTC->DR |= ((ptrRTC_Handler->diaSemana) << RTC_DR_WDU_Pos);

	//6.3.4 Dividir el año en unidades y decenas
	dividirValor(ptrRTC_Handler->año);

	//Cargar las unidades del año
	RTC->DR |= (unidades << RTC_DR_YU_Pos);

	//Cargar las decenas del año
	RTC->DR |= (decenas << RTC_DR_YT_Pos);

	//6.4 CONFIGURAR EL TIEMPO. En el TR-> segundos, minutos, horas

	//6.4.1 Dividir los segundos en unidades y decenas
	dividirValor(ptrRTC_Handler->segundo);
	//Limpiar los bits de los segundos
	RTC->TR &= ~RTC_TR_SU;
	RTC->TR &= ~RTC_TR_ST;

	//Cargar las unidades de los segundos
	RTC->TR |= (unidades << RTC_TR_SU_Pos);

	//Cargar las decenas de los segundos
	RTC->TR |= (decenas << RTC_TR_ST_Pos);

	//6.4.2 Dividir los minutos en unidades y decenas
	dividirValor(ptrRTC_Handler->minuto);
	//Limpiar los bits de los minutos
	RTC->TR &= ~RTC_TR_MNU;
	RTC->TR &= ~RTC_TR_MNT;

	//Cargar las unidades de los minutos
	RTC->TR |= (unidades << RTC_TR_MNU_Pos);

	//Cargar las decenas de los minutos
	RTC->TR |= (decenas << RTC_TR_MNT_Pos);

	//6.4.3 Dividir las horas en unidades y decenas
	dividirValor(ptrRTC_Handler->hora);
	//Limpiar los bits de las horas
	RTC->TR &= ~RTC_TR_HU;
	RTC->TR &= ~RTC_TR_HT;

	//Cargar las unidades de las horas
	RTC->TR |= (unidades << RTC_TR_HU_Pos);

	//Cargar las decenas de las horas
	RTC->TR |= (decenas << RTC_TR_HT_Pos);


	//5. Finalizar el modo inicialización
	RTC->ISR &= ~RTC_ISR_INIT;

}

/*
 * Función para dividir un valor en unidades y decenas. Necesario para cargar
 * los elementos del time y date register.
 */
void dividirValor(uint8_t valor){
	decenas = valor/10;

	unidades = valor - (decenas*10);
}

//Función para leer la hora del RTC
uint8_t horasRTC(void){
	uint8_t decenasHoras = 0;
	uint8_t unidadesHoras = 0;
	uint8_t hora = 0;

	//Leer el registro de las decenas de la hora
	decenasHoras = ((RTC->TR & RTC_TR_HT) >> RTC_TR_HT_Pos);

	//Leer el registro de las unidades de la hora
	unidadesHoras = ((RTC->TR & RTC_TR_HU) >> RTC_TR_HU_Pos);

	//Acomodamos las unidades y las decenas para obtener el valor real
	hora = (decenasHoras*10) + unidadesHoras;

	return hora;

}

//Función para leer los minutos del RTC
uint8_t minutosRTC(void){
	uint8_t decenasMin = 0;
	uint8_t unidadesMin = 0;
	uint8_t minutos = 0;

	//Leer el registro de las decenas de los minutos
	decenasMin = ((RTC->TR & RTC_TR_MNT) >> RTC_TR_MNT_Pos);

	//Leer el registro de las unidades de los minutos
	unidadesMin = ((RTC->TR & RTC_TR_MNU) >> RTC_TR_MNU_Pos);

	//Acomodamos las unidades y las decenas para obtener el valor real

	minutos = (decenasMin*10) + unidadesMin;

	return minutos;
}

//Función para leer los segundos del RTC
uint8_t segundosRTC(void){
	uint8_t decenasSeg = 0;
	uint8_t unidadesSeg = 0;
	uint8_t segundos = 0;

	//Leer el registro de las decenas de los segundos
	decenasSeg = ((RTC->TR & RTC_TR_ST) >> RTC_TR_ST_Pos);

	//Leer el registro de las unidades de los segundos
	unidadesSeg = ((RTC->TR & RTC_TR_SU) >> RTC_TR_SU_Pos);

	//Acomodamos las unidades y las decenas para obtener el valor real
	segundos = (decenasSeg*10) + unidadesSeg;

	return segundos;
}

//Función para leer si la hora es AM o PM
uint8_t AM_PM_RTC(void){
	uint8_t AM_PM = 0;

	AM_PM = ((RTC->TR & RTC_TR_PM) >> RTC_TR_PM_Pos);

	//Si AM_PM = 0 indica AM
	//Si AM_PM = 1 indica PM
	return AM_PM;

}

//Función para leer el formato de la hora
uint8_t formatoHoraRTC(void){
	uint8_t formato = 0;

	formato = ((RTC->CR & RTC_CR_FMT) >> RTC_CR_FMT_Pos);

	//Si formato = 0 indica 24 horas
	//Si formato = 1 indica 12 horas

	return formato;
}

//Función para leer el año del RTC
uint8_t añoRTC(void){
	uint8_t decenasAño = 0;
	uint8_t unidadesAño = 0;
	uint8_t año = 0;

	//Leer el registro de las decenas del año
	decenasAño = ((RTC->DR & RTC_DR_YT) >> RTC_DR_YT_Pos);

	//Leer el registro de las unidades del año
	unidadesAño = ((RTC->DR & RTC_DR_YU) >> RTC_DR_YU_Pos);

	//Acomodamos las unidades y las decenas para obtener el valor real
	año = (decenasAño*10) + unidadesAño;

	return año;
}

//Función para leer el mes del RTC
uint8_t mesRTC(void){
	uint8_t decenasMes = 0;
	uint8_t unidadesMes = 0;
	uint8_t mes = 0;

	//Leer el registro de las decenas del mes
	decenasMes = ((RTC->DR & RTC_DR_MT) >> RTC_DR_MT_Pos);

	//Leer el registro de las unidades del mes
	unidadesMes = ((RTC->DR & RTC_DR_MU) >> RTC_DR_MU_Pos);

	//Acomodamos las unidades y las decenas para obtener el valor real
	mes = (decenasMes*10) + unidadesMes;

	return mes;

}

//Función para leer el dia de la semana del RTC
uint8_t diaRTC(void){
	uint8_t decenasDia = 0;
	uint8_t unidadesDia = 0;
	uint8_t dia = 0;

	//Leer el registro de las decenas del dia
	decenasDia = ((RTC->DR & RTC_DR_DT) >> RTC_DR_DT_Pos);

	//Leer el registro de las unidades del dia
	unidadesDia = ((RTC->DR & RTC_DR_DU) >> RTC_DR_DU_Pos);

	//Acomodamos las unidades y las decenas para obtener el valor real
	dia = (decenasDia*10) + unidadesDia;

	return dia;
}

//Función para leer el día de la semana
uint8_t diaSemanaRTC(void){
	uint8_t diaSemana = 0;

	diaSemana = ((RTC->DR & RTC_DR_WDU) >> RTC_DR_WDU_Pos);

	return diaSemana;

}

