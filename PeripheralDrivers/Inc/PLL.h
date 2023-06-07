/*
 * PLL.h
 *
 *  Created on: May 22, 2023
 *      Author: isabel
 */

#ifndef PLL_H_
#define PLL_H_

#include <stm32f4xx.h>

#define PLL_FRECUENCIA_80MHZ		1
#define PLL_FRECUENCIA_100MHZ		2

#define CLOCK_SIGNAL_PLL			3
#define CLOCK_SIGNAL_HSI			4
#define CLOCK_SIGNAL_LSE			5

#define MCO1_NOPRESCALER			1
#define MCO1_PRESCALERX2			2
#define MCO1_PRESCALERX3			3
#define MCO1_PRESCALERX4			4
#define MCO1_PRESCALERX5			5


//Función que configura la velocidad del micro a 80MHz
void configPLL(uint8_t frecuencia);
uint8_t getConfigPLL(uint8_t PLLP, uint8_t PLLN);
void configMCO1(uint8_t clockSignal, uint8_t prescaler);

#endif /* PLL_H_ */
