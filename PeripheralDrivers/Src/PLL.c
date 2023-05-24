/*
 * PLL.c
 *
 *  Created on: May 22, 2023
 *      Author: isabel
 */

#include <stm32f4xx.h>
#include "PLL.h"


void configPLL80MHz(void){


	// Apagar el PLL antes de configurar
	RCC->CR &= ~(RCC_CR_PLLON);

	//Encender el HSI (el que alimenta al PLL)
	RCC->CR |= RCC_CR_HSION;


	//Seleccionar HSI -> 16MHz como reloj de entrada al PLL
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC);


	/*  Configurar el valor de PLLM
	VOC_input_frequency = PLL_input_clock_frequency / PLLM
	Teniendo en cuenta que PLL_input_clock_frequency = 16MHz y que
	VOC_input_frequency se recomienda que sea = 2MhZ
	 */

	//Limpiamos el registro
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);

	//Cargamos el valor de 8 al registro
	RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLM_Pos);

	/*
	Configurar el PLLN
	VCO_output_frequency = VCO_input_frequency*PLLN
	Sabiendo que VCO_output_frequency debe ser mayor a 100 y
	menor a 434 MHz
	 */

	//Limpiar el registro
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);

	//Cargamos el valor de 80 al registro
	RCC->PLLCFGR |= (80 << RCC_PLLCFGR_PLLN_Pos);

	/*
	 Configurar el PLLP
	 PLL_output_clock_frequency = VCO_frequency / PLL
	 Teniendo en cuenta que, PLL_output_clock_frequency = 80MHz
	 VCO_frequency = 160 MHz
	 */

	//Para cargar el valor de 2, se debe limpiar el registro (0b00)
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);


	/*
	 * Configurar el tiempo de lectura de la memoria flash para
	 * que sea compatible con la velocidad del microcontrolador
	 */

	//Limpiar los registros

	// La macro dice 7 -> 111 y debería ser un F ->1111
	FLASH->ACR &= ~(0xF << FLASH_ACR_LATENCY_Pos);

	//Configurar 2WS en el registro de latencia
	FLASH->ACR |= (0x2 << FLASH_ACR_LATENCY_Pos);


	/*
	 * Para observar la frecuencia del microcontrolador directamente
	 * se puede usar la función alternativa MCO1 o MCO2.
	 * Ya que el analizador de señales no registra frecuencias de 80MHz
	 * se debe prescalar el MCO antes de la salida para poder detectarlo
	 */

	//Limpiar los bits correspondientes
	RCC->CFGR &= ~(RCC_CFGR_MCO1);

	//Configurar para que la salida sea la del PLL
	RCC->CFGR |= (0x3 << RCC_CFGR_MCO1_Pos);

	//Limpiar los bits del prescaler
	RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);

	//Divido la frecuencia entre 5
	RCC->CFGR |= (0x7 << RCC_CFGR_MCO1PRE_Pos);


	//Activar el PLL
	RCC->CR |= (RCC_CR_PLLON);

	//Esperar a que el PLL esté listo
	while(!(RCC->CR & (RCC_CR_PLLRDY))){
		__NOP();
	}


	//Configurar para que el reloj del sistema sea el del PLL

	//Limpiar las posiciones
	RCC->CFGR &= ~(RCC_CFGR_SW);

	//Configurar el PLL como reloj principal
	RCC->CFGR |= (RCC_CFGR_SW_1);


	/*
	 * Teniendo en cuenta que el APB1 funciona a máximo 50MHz,
	 * hay que dividir la frecuencia a la que estamos trabajando
	 * (80MHz de modo que este bus puedaa trabajar adecuadamente)
	 */

	//Configurar el prescales del AHB1 para que no tenga prescaler
	RCC->CFGR &= ~(RCC_CFGR_HPRE);

	//Limpiar los registros
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);

	//Para dividir la frecuencia entre 2 (80MHz/2MHz) es necesario
	//escribir un 0b100 en la posición de registro adecuada
	RCC->CFGR |= (RCC_CFGR_PPRE1_2);


}

