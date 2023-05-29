/*
 * LcdDriver.h
 *
 *  Created on: May 27, 2023
 *      Author: isabel
 */

#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"


#define RS		0b00000001
#define RW		0b00000010
#define	E		0b00000100
#define LED		0b00001000
#define D4		0b00010000
#define D5		0b00100000
#define D6		0b01000000
#define	D7		0b10000000

////Enviar comandos a la LCD
//void LCD_comandos (char dir, char comando);
///*
// * Hay que escribir sobre el PCF, en la dirección (parametro de la función) el
// * comando (parametro de la función)
// * Primero configurar para que se envíe el parámetro 0 para limiar
// *
// * Hacer un delay 100 ms
// *
// * El comando que manda el usuario, hay que partirlo en 2, 4 bits.
// * Porque el modulo solo conecta los 4 bits más significativos
// *
// */

#define COMANDO		0
#define DATO		1

#define CLEAR		0x01

#define SET			0x03


void particion(I2C_Handler_t *ptrHandlerI2C, uint8_t tipo, uint8_t datoOcmd);
void manejoDatos_LCD(I2C_Handler_t *ptrHandlerI2C, uint8_t tipo, uint8_t datoOcmd);
void escribir_LCD (I2C_Handler_t *ptrHandlerI2C, char dato);
void Init_LCD(I2C_Handler_t *ptrHandlerI2C);
void comandos_LCD(I2C_Handler_t *ptrHandlerI2C, uint8_t comando);
void nuevoDato_LCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dato);
void LCD_XY(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void LCD_dato_XY_(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y, uint8_t caracter);
void LCD_out_Msg(I2C_Handler_t *ptrHandlerI2C, char Msg[]);
void LCD_Out_Msg_XY(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y, char Msg[]);
void LCD_Out_Msg_XY(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y, char Msg[]);





#endif /* LCDDRIVER_H_ */
