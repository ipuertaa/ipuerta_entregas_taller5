/*
 * PLL.c
 *
 *  Created on: May 22, 2023
 *      Author: isabel
 */

#include <stm32f4xx.h>
#include "PLL.h"


void configPLL(uint8_t frecuencia){


	// Apagar el PLL antes de configurar
	RCC->CR &= ~(RCC_CR_PLLON);

	/*
	 * Existen 3 fuentes de reloj principales:
	 * - HSE: High Speed External Clock: osciladores externos
	 * - HSI: oscilador interno de 16MHz
	 * - PLL: mediante este se puede modificar la velocidad del MCU
	 */

	/*
	 * 	Sobre la calibración del HSI
	 * 	- La calibración de fabrica se guarda en el RCC clock control register RCC_CR
	 * 	en el HSICAL
	 *
	 * 	El fabricante garantiza que, cuando el producto sale de fábrica tiene una incertidumbre
	 * 	del 1%, por lo que, siendo el HSI de 16 MHz puede variar entre 15840 kHz y 16160 kHz
	 *
	 * 	- La calibración también se hace en el RCC_CR en los bits HSITRIM
	 * 	- Mediante la bandera del HSIRDY se indica si el HSI es estable o no.
	 *
	 * 	El registro HSITRIM  tiene 5 bits. Permite 32 configuraciones. El valor por defecto es 16
	 * 	A medida que se incrementa el valor del trim, la frecuencia del reloj aumenta.
	 * 	El HSITRIM vuelve a 16 cada vez que se hace reset.
	 *
	 * 	Cuando se escribe en el HSITRIM, el HSICAL se actualiza con la suma entre el HSITRIM y el valor
	 * 	de fábrica.
	 * 	Cuando se aumenta 0x01, se incrementa el valor de la frecuencia del oscilador aproximadamente 0.3%
	 *
	 * 	AL escribir un valor de calibración entre 17 y 31, aumenta la frecuencia del HSI
	 * 	Al escribir un valor de calibración entre 0 y 15, disminuye la frecuencia del HSI
	 *

	 */

	RCC->CR &= ~(RCC_CR_HSITRIM);
	RCC->CR |= (0xD << RCC_CR_HSITRIM_Pos);

	//Espero a que esté estable el HSI
	while(!(RCC->CR & (RCC_CR_HSIRDY))){
		__NOP();
	}

	//Encender el HSI (el que alimenta al PLL)
	RCC->CR |= RCC_CR_HSION;


	//Seleccionar HSI -> 16MHz como reloj de entrada al PLL
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC);


	/*  Configurar el valor de PLLM
	 *  El PLLM es el factor de división de la entrada de reloj del PLL
	 *  Éste divide la frecuencia de entrada (HSI) de modo que, la frecuencia
	 *  de entrada al VCO sea de 2 MHz.
	 *
	 *  VOC_input_frequency = PLL_input_clock_frequency / PLLM
	 *  Teniendo en cuenta que PLL_input_clock_frequency = 16MHz y que
	 *  VOC_input_frequency se recomienda que sea = 2MMHz
	 *
	 *  PLLM = PLL_input_clock_frequency/VCO_input_frequency
	 *
	 */

	//Limpiamos el registro
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);

	//Cargamos el valor de 8 al registro
	RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLM_Pos);

	/*
	 * Configurar el PLLN:
	 * El PLLN es el factor que multiplica el VOC_output_frequency
	 * Se debe configurar el PLLN de modo que VCO_output_frequency sea
	 * mayor a 100 y menor a 432 MHz
	 *
	 * VCO_output_frequency = VCO_input_frequency*PLLN
	 * VCO_output_frequency = 2 MHz * PLLN
	 *
	 * Para configurar una frecuencia de 80 MHz
	 * Si PLLN = 80, VCO_output_frequency = 160 MHz
	 *
	 * Para configurar una frecuencia de 100 MHz
	 * Si PLLN = 100, VCO_output_frequency = 200 MMhz
	 */

	//Limpiar el registro
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);

	if(frecuencia == PLL_FRECUENCIA_80MHZ){

		//Cargamos el valor de 80 al registro
		RCC->PLLCFGR |= (80 << RCC_PLLCFGR_PLLN_Pos);
	}
	else if(frecuencia == PLL_FRECUENCIA_100MHZ){

		// Cargamos el valor de 100 al registro
		RCC->PLLCFGR |= (100 << RCC_PLLCFGR_PLLN_Pos);
	}
	else{
		__NOP();
	}



	/*
	 * Configurar el PLLP:
	 * Este es el factor que garantiza el main clock System. Controla la frecuencia
	 * final del PLL
	 * NO se puede exceder de los 100 MHz
	 *
	 * PLL_output_clock_frequency = VCO_frequency / PLLP
	 * Teniendo en cuenta que:
	 * VCO_frequency = 160 MHz
	 * PLLP = VOC_output_frequency / PLL_output_clock_frequency
	 *
	 * 	- Si PLL_output_clock_frequency = 80 MHz:
	 * 		PLLLP = 160 MHz / 80 MHz -> PLLP = 2
	 *
	 * 	- Si PLL_output_clock_frequency = 100 MHz:
	 * 		PLLP = 200 MHz / 100 MMz -> PLLP = 2

	 */

	//Para cargar el valor de 2, se debe limpiar el registro (0b00)
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);


	/*
	 * Configurar el tiempo de lectura de la memoria flash para
	 * que sea compatible con la velocidad del microcontrolador
	 *
	 * Para un CPU Clock entre 64 y 90 MHz: wait states = 2
	 * Para un CPU Clock entre 91 y 100 MHz: wait states = 3
	 */

	//Limpiar los registros

	// La macro dice 7 -> 111 y debería ser un F ->1111
	FLASH->ACR &= ~(0xF << FLASH_ACR_LATENCY_Pos);

	if(frecuencia == PLL_FRECUENCIA_80MHZ){

		//Configurar 2WS en el registro de latencia
		FLASH->ACR |= (0x2 << FLASH_ACR_LATENCY_Pos);
	}
	else if(frecuencia == PLL_FRECUENCIA_100MHZ){

		//Configurar 3WS en el registro de latencia
		FLASH->ACR |= (0x3 << FLASH_ACR_LATENCY_Pos);
	}
	else{
		__NOP();
	}

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

	//Configurar el prescaler del AHB1 para que no tenga prescaler
	RCC->CFGR &= ~(RCC_CFGR_HPRE);

	//El APB2 puede trabajar a 100 MHz por lo tanto, no es necesario prescalarlo
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);

	//Limpiar los registros
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);

	//El APB1 no puede exceder los 50 MHz, por lo tanto, se divide la frecuencia
	//entre 2. Para que pueda funcionar tanto a 80 MHz como a 100 MHz
	//Para dividir la frecuencia entre 2 es necesario
	//escribir un 0b100 en la posición de registro adecuada
	RCC->CFGR |= (RCC_CFGR_PPRE1_2);



}

uint8_t getConfigPLL(uint8_t PLLP, uint8_t PLLN){

	uint8_t Fsalida = 0;
	Fsalida = (PLLN*2)/PLLP;
	return Fsalida;
}

void configMCO1(uint8_t clockSignal, uint8_t prescaler){

	/*
	 * Para observar la frecuencia del microcontrolador directamente
	 * se puede usar la función alternativa MCO1 o MCO2.
	 *
	 * Por este pin, se puede configurar para visualizar las señales de reloj
	 * internas del MCU, ya sea HSI, LSE y el PLL:
	 * Además, se puede hacer prescaler a la frecuencia para poder visualizarla
	 * apropiadamente con instrumentos de medida.
	 *
	 */

	//Limpiar los bits de configuración del MCO1
	RCC->CFGR &= ~(RCC_CFGR_MCO1);

	//Limpiar los bits del prescaler
	RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);

	if(clockSignal == CLOCK_SIGNAL_PLL){

		//Configurar para que la salida sea la del PLL
		RCC->CFGR |= (0x3 << RCC_CFGR_MCO1_Pos);
	}
	else if(clockSignal == CLOCK_SIGNAL_HSI){

		//Configurar para que la salida sea la del HSI
		RCC->CFGR &= ~(RCC_CFGR_MCO1);
	}
	else if(clockSignal == CLOCK_SIGNAL_LSE){

		//Configurar para que la salida sea la del LSE
		RCC->CFGR |= (0x1 << RCC_CFGR_MCO1_Pos);
	}
	else{
		__NOP();
	}


	switch(prescaler){
	case MCO1_NOPRESCALER:{
		// No se divide. Se dejan los bits en cero
		RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
		break;
	}
	case MCO1_PRESCALERX2:{

		//Escribir un 100 = 0x4 en el registro
		RCC->CFGR |= (0x4 << RCC_CFGR_MCO1PRE_Pos);
		break;
	}
	case MCO1_PRESCALERX3:{

		//Escribir un 101 = 0x5  en el registro
		RCC->CFGR |= (0x5 << RCC_CFGR_MCO1PRE_Pos);
		break;
	}
	case MCO1_PRESCALERX4:{

		//Escribir un 110 = 0x6  en el registro
		RCC->CFGR |= (0x6 << RCC_CFGR_MCO1PRE_Pos);
		break;
	}
	case MCO1_PRESCALERX5:{

		//Escribir un 111 = 0x7 en el registro
		RCC->CFGR |= (0x7 << RCC_CFGR_MCO1PRE_Pos);
		break;
	}
	default:{
		__NOP();
		break;
	}

	}	//Fin prescaler

}	//Fin configMCO1

