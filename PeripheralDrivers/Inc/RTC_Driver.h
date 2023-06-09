/*
 * RTC_Driver.h
 *
 *  Created on: Jun 7, 2023
 *      Author: isabel
 */

#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_

#include "stm32f4xx.h"

/*
 * Mediante el RTC se puede configurar una fecha y hora actual y él seguirá contando
 * hasta que se interrumpa la alimentación del microcontrolador
 */

#define LUNES				1
#define MARTES				2
#define MIERCOLES			3
#define JUEVES				4
#define VIERNES				5
#define SABADO				6
#define DOMINGO				7


#define ENERO				0
#define FEBRERO				1
#define MARZO				2
#define ABRIL				3
#define MAYO				4
#define JUNIO				5
#define JULIO				6
#define AGOSTO				7
#define SEPTIEMBRE			8
#define OCTUBRE				9
#define NOVIEMBRE			10
#define DICIEMBRE			11

#define FORMATO_12HORAS		0
#define FORMATO_24HORAS		1
#define AM					2
#define PM					3



typedef struct{
	uint8_t diaSemana;		//Lunes, martes, miércoles
	uint8_t dia;
	uint8_t mes;
	uint8_t año;
	uint8_t formatoHora;
	uint8_t Am_pm;
	uint8_t hora;
	uint8_t minuto;
	uint8_t segundo;
} RTC_Handler_t;

void configRTC (RTC_Handler_t *ptrRTC_Handler);
void dividirValor(uint8_t valor);
uint8_t horasRTC(void);
uint8_t minutosRTC(void);
uint8_t segundosRTC(void);
uint8_t AM_PM_RTC(void);
uint8_t formatoHoraRTC(void);
uint8_t añoRTC(void);
uint8_t diaRTC(void);
uint8_t diaSemanaRTC(void);


#endif /* RTC_DRIVER_H_ */
