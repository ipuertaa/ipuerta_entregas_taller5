/*
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 3, 2023
 *      Author: Isabel Puerta
 * En este archivo se definen las directivas de pre-procesamiento para manipular
 * los registros para el uso del HAL
 */

#ifndef STM32F411XX_HAL_H_
#define STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED  16000000       //Value form the main clock signal (HSI -> High Speed Internal
#define HSE_CLOCK_SPEED  4000000        //Value form the main clock signal (HSE -> High Speed External

#define NOP()   asm ("NOP")
#define __weak  __attribute__((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, memory map, figure 14
 * Remember: 1KByte = 1024 bites
 */

#define FLASH_BASE_ADDR  0x08000000U   //Esta es la memoria del programa, 512KB
#define SRM_BASE_ADDR    0x20000000U   //Esta es la memoria RAM, 128KB

/* Agregamos la dirección de memoria base para cada uno de los perifericos del micro.
 * En el datasheet, fig 14 se encuentra el mapa de los buses
 *     APBx -> Advance Peripherical Bus
 *     AHBx -> Advance Hig-Performance Bus
 */

#define APB1_BASE_ADDR  0x40000000U
#define APB2_BASE_ADDR	0x40010000U
#define AHB1_BASE_ADDR	0x40020000U
#define AHB2_BASE_ADDR	0x50000000U

/*
 * Se hace lo mismo con cada una de las posiciones de memoria de cada uno de los perifericos
 * descritos en la tabla 1 del manual de referencia.
 */

// Posiciones de memoria para perifericos del AHB2

#define USB_OTG_FS_BASE_ADDR	(AHB2_BASE_ADDR + 0x 0000U)

//Posiciones de memoria para periféricos del AHB1.

#define RCC_BASE_ADDR		(AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR		(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR		(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR		(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR		(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR		(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR		(AHB1_BASE_ADDR + 0x0000U)

// Macros genéricos

#define ENABLE			1
#define DISABLE			0
#define SET				ENABLE
#define CLEAR			DISABLE
#define RESET			DISABLE
#define GPIO_PIN_SET	SET
#define GPIO_PIN_RESET	RESET
#define FLAG_SET		SET
#define FLAG_RESET		RESET
#define I2C_WRITE		0
#define I2C_READ		1

/*   Inicio de la descripción de los elementos que componen el periférico
 * Se define la estructura de datos que representa a cada uno de los registros
 * que componen el periférico RCC
 *
 * La posición de memoria (offset) debe encajar perfectamente con la posición de memoria
 * indicada en la hoja de datos del equipo. Los elementos reservados también están presentes
 */

typedef struct
{
	volatile uint32_t CR;				//Clock control register					ADDR_OFFSET:	0x00
	volatile uint32_t PLLCFGR;			//PLL configuration register				ADDR_OFFSET:	0x04
	volatile uint32_t CFGR;				//Clock configuration register				ADDR_OFFSET:	0x08
	volatile uint32_t CIR;				//Clock interrupt register					ADDR_OFFSET:	0x0C
	volatile uint32_t AHB1RSTR;			//AHB1 Peripheral reset register			ADDR_OFFSET:	0x10
	volatile uint32_t AHB2RSTR;			//AHB2 peripheral reset register			ADDR_OFFSET:	0x14
	volatile uint32_t reserved0;		//reserved									ADDR_OFFSET:	0x18
	volatile uint32_t reserved1;		//reserved									ADDR_OFFSET:	0x1C
	volatile uint32_t APB1RSTR;			//APB1 peripheral reset register			ADDR_OFFSET:	0x20
	volatile uint32_t APB2RSTR;			//APB2 peripheral reset register			ADDR_OFFSET:	0x24
	volatile uint32_t reserved2;		//reserved									ADDR_OFFSET:	0x28
	volatile uint32_t reserved3;		//reserved									ADDR_OFFSET:	0x2C
	volatile uint32_t AHB1ENR;			//AHB1 peripheral clock enable register		ADDR_OFFSET:	0x30
	volatile uint32_t AHB2ENR;			//AHB2 peripheral clock enable register		ADDR_OFFSET:	0x34
	volatile uint32_t reserved4;		//reserved									ADDR_OFFSET:	0x38
	volatile uint32_t reserved5;		//reserved									ADDR_OFFSET:	0x3C
	volatile uint32_t APB1ENR;			//APB1 peripheral clock enable register		ADDR_OFFSET:	0x40
	volatile uint32_t APB2ENR;			//APB2 peripheral clock enable register		ADDR_OFFSET:	0x44
	volatile uint32_t reserved6;		//reserved									ADDR_OFFSET:	0x48
	volatile uint32_t reserved7;		//reserved

}
#endif /* STM32F411XX_HAL_H_ */
