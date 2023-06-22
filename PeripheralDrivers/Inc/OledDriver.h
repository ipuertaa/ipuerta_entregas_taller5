/*
 * OledDriver.h
 *
 *  Created on: Jun 2, 2023
 *      Author: isabel
 */

#ifndef OLEDDRIVER_H_
#define OLEDDRIVER_H_
#define oLEDAddress 			0x3C
#define CONTROL_BYTE_COMANDO	0x00
#define CONTROL_BYTE_DATO		0x40
#define CONTROL_BYTE_DATO_M		0b110000


#include <stm32f4xx.h>
#include "I2CDriver.h"


//Decinición de funciones:

void comandoOled(I2C_Handler_t *ptrHandlerI2C, uint8_t comando);
void initOled(I2C_Handler_t *ptrHandlerI2C);
void sendDataByteOled(I2C_Handler_t *ptrHandlerI2C, uint8_t data);
void sendDataBytesOled(I2C_Handler_t *ptrHandlerI2C, char* variosDatos);
void setColumnaOled(I2C_Handler_t *ptrHandlerI2C, uint8_t columna);
void setPaginaOled(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina);
void setFilaInicio(I2C_Handler_t *ptrHandlerI2C, uint8_t StartLine);
void clearScreenOLED(I2C_Handler_t *ptrHandlerI2C);
char* ASCIItoChar(char caracter);
void EscribirPaginaOled(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina, char* *data);
void OLED_print_msg_pag(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina, char *msgToSendOLED);
void OLED_print_msg(I2C_Handler_t *ptrHandlerI2C, char *msgToSendOLED);
void OLED_print_msg_pag_inicio(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina, char *msgToSendOLED, uint8_t inicio);

#endif /* OLEDDRIVER_H_ */
