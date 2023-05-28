/*
 * SysTickDriver.h
 *
 *  Created on: Apr 27, 2023
 *      Author: isabel
 */

#ifndef SYSTICKDRIVER_H_
#define SYSTICKDRIVER_H_

#include <stm32f4xx.h>

#define SYSTICK_LOAD_VALUE_16MHz_1ms		16000		// Numero de ciclos en 1ms
#define SYSTICK_LOAD_VALUE_100MHz_1ms		1000000		// Numero de ciclos en 1ms
#define SYSTICK_LOAD_VALUE_81MHz_1ms		81000		// Numero de ciclos en 1ms
#define SYSTICK_LOAD_VALUE_80MHz_1ms		80000

#define HSI_CLOCK_CONFIGURED				0
#define HSE_CLOCK_CONFIGURED				1
#define PLL_100MHz_CLOCK_CONFIGURED			2
#define PLL_81MHz_CLOCK_CONFIGURED			3
#define PLL_80MHz_CLOCK_CONFIGURED			4



void config_SysTick_ms(uint8_t systemClock);
uint64_t getTicks_ms(void);
void delay_ms(uint32_t wait_time_ms);


#endif /* SYSTICKDRIVER_H_ */
