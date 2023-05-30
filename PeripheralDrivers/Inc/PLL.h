/*
 * PLL.h
 *
 *  Created on: May 22, 2023
 *      Author: isabel
 */

#ifndef PLL_H_
#define PLL_H_

#include <stm32f4xx.h>

//Función que configura la velocidad del micro a 80MHz
void configPLL80MHz(void);
uint8_t getConfigPLL(uint8_t PLLP, uint8_t PLLN);


#endif /* PLL_H_ */
