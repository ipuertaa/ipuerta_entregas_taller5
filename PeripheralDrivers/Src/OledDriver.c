/*
 * OledDriver.c
 *
 *  Created on: Jun 2, 2023
 *      Author: isabel
 */

#include <stm32f4xx.h>
#include "OledDriver.h"
#include <stdint.h>
#include "I2CDriver.h"
#include "SysTickDriver.h"


// Caracteres del alfabeto
char alphabet[40][8] = {
// Letter A
{0x00, 0b11111100, 0b00010010, 0b00010010, 0b00010010, 0b11111100, 0x00, 0x00},
// Letter B
{0x00, 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00},
// Letter C
{0x00, 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01000100, 0x00, 0x00},
// Letter D
{0x00, 0b11111110, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0x00, 0x00},
// Letter E
{0x00, 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b10000010, 0x00, 0x00},
// Letter F
{0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b00000010, 0x00, 0x00},
// Letter G
{0x00, 0b01111100, 0b10000010, 0b10000010, 0b10100010, 0b01100100, 0x00, 0x00},
// Letter H
{0x00, 0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0x00, 0x00},
// Letter I
{0x00, 0b10000010, 0b10000010, 0b11111110, 0b10000010, 0b10000010, 0x00, 0x00},
// Letter J
{0x00, 0b01100000, 0b10000000, 0b10000000, 0b10000000, 0b01111110, 0x00, 0x00},
// Letter K
{0x00, 0b11111110, 0b00010000, 0b00101000, 0b01000100, 0b10000010, 0x00, 0x00},
// Letter L
{0x00, 0b11111110, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0x00, 0x00},
// Letter M
{0x00, 0b11111110, 0b00000100, 0b00001000, 0b00000100, 0b11111110, 0x00, 0x00},
// Letter N
{0x00, 0b11111110, 0b00001000, 0b00010000, 0b00100000, 0b11111110, 0x00, 0x00},
// Letter O
{0x00, 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0x00, 0x00},
// Letter P
{0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b00001100, 0x00, 0x00},
// Letter Q
{0x00, 0b01111100, 0b10000010, 0b10100010, 0b01000010, 0b10111100, 0x00, 0x00},
// Letter R
{0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b11101100, 0x00, 0x00},
// Letter S
{0x00, 0b01001100, 0b10010010, 0b10010010, 0b10010010, 0b01100100, 0x00, 0x00},
// Letter T
{0x00, 0b00000010, 0b00000010, 0b11111110, 0b00000010, 0b00000010, 0x00, 0x00},
// Letter U
{0x00, 0b01111110, 0b10000000, 0b10000000, 0b10000000, 0b01111110, 0x00, 0x00},
// Letter V
{0x00, 0b00111110, 0b01000000, 0b10000000, 0b01000000, 0b00111110, 0x00, 0x00},
// Letter W
{0x00, 0b01111110, 0b10000000, 0b01100000, 0b10000000, 0b01111110, 0x00, 0x00},
// Letter X
{0x00, 0b11000110, 0b00101000, 0b00010000, 0b00101000, 0b11000110, 0x00, 0x00},
// Letter Y
{0x00, 0b00000110, 0b00001000, 0b11110000, 0b00001000, 0b00000110, 0x00, 0x00},
// Letter Z
{0x00, 0b11000010, 0b10100010, 0b10010010, 0b10001010, 0b10000110, 0x00, 0x00},
// " "
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
// Num 0
{0x00, 0b01111100, 0b10100010, 0b10010010, 0b10001010, 0b01111100, 0x00, 0x00},
//Num 1
{0x00, 0b00000000, 0b10000100, 0b11111110, 0b10000000, 0b00000000, 0x00, 0x00},
//Num 2
{0x00, 0b11000100, 0b10100010, 0b10010010, 0b10010010, 0b10001100, 0x00, 0x00},
//Num 3
{0x00, 0b01000100, 0b10000010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00},
//Num 4
{0x00, 0b00110000, 0b00101000, 0b00100100, 0b00100010, 0b11111110, 0x00, 0x00},
//Num 5
{0x00, 0b01001110, 0b10001010, 0b10001010, 0b10001010, 0b01110010, 0x00, 0x00},
//Num 6
{0x00, 0b01111100, 0b10010010, 0b10010010, 0b10010010, 0b01100100, 0x00, 0x00},
//Num 7
{0x00, 0b00000010, 0b11100010, 0b00010010, 0b00001010, 0b00000110, 0x00, 0x00},
//Num 8
{0x00, 0b01101100, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00},
//Num 9
{0x00, 0b01001100, 0b10010010, 0b10010010, 0b10010010, 0b01111100, 0x00, 0x00},
//Char :
{0x00, 0x00, 0x00, 0x00, 0b01001000, 0x00, 0x00, 0x00},
//Char =
{0x00, 0x00, 0b00101000, 0b00101000, 0b00101000, 0x00, 0x00, 0x00},
//Char $
//{0x00, 0x00, 0x00, 0x00, 0x7D, 0x00, 0x00, 0x00} ¡
{0x00, 0x00, 0x00, 0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010}
};


//Función para escribir comandos
void comandoOled(I2C_Handler_t *ptrHandlerI2C, uint8_t comando){

	//1. Generamos la condición de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de escribir
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//3. Enviamos el control byte
	i2c_sendMemory_Address(ptrHandlerI2C, CONTROL_BYTE_COMANDO);

	//4. Enviamos el valor que deseamos escribir en el registro seleccionado
	i2c_sendDataByte(ptrHandlerI2C, comando);

	//5. Generamos la condición de STOP para que el valor se detenga despúes de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

}
//Función de inicialización
void initOled(I2C_Handler_t *ptrHandlerI2C){

	/*
	 * Para poder indicar si se va a enviar un comando o un dato se debe enviar un byte de control, que tiene
	 * la sigueinte estructura:
	 * 				C0 DC 0 0 0 0 0 0
	 * C0 = 0 -> Indica que es el último byte de control. Los bytes que siguen después son de datos
	 * C0 = 1 -> No es el último byte de control.
	 * DC = 0 -> Indica que se va a enviar un comando
	 * DC = 1 -> Indica que se va a enviar un dato
	 *
	 */

	/*
	 * 			Secuencia de inicialización
	 *
	 *	1. Delay
	 *	2. Apagar el display: 0xAE- > 1 0 1 0 1 1 1 0
	 *	3. Enviar comando 0xD5 ->     1 1 0 1 0 1 0 1
	 *	4. Enviar comando 0x80 -> 	  1 0 0 0 0 0 0 0
	 *	5. Enviar comando 0xA8 ->     1 0 1 0 1 0 0 0
	 *	6. Enviar comando 0x3F -> 	  1 1 1 1 1 1 0 0
	 *	7. Enviar comando 0xD3 ->     1 1 0 1 0 0 1 1
	 *	8. Enviar comando 0x00 ->	  0 0 0 0 0 0 0 0
	 *  9. Start line 0x40 ->         0 1 0 0 0 0 0 0
	 *  10. Charge Pump 0xAD ->       1 0 1 0 1 1 0 1
	 *  11. Charge Pump 0X8B ->       1 0 0 0 1 0 1 1
	 *  12. Segment re map 0xA1 ->    1 0 1 0 0 0 0 1
	 *  13. Out scan direc 0xC8 ->	  1 1 0 0 1 0 0 0
	 *  14. Set pins 0xDA  ->         1 1 0 1 1 0 1 0
	 *  15. Set pins 0x12 ->          0 0 0 1 0 0 1 0
	 *  16. Contraste 0x81 ->         1 0 0 0 0 0 0 1
	 *  17. Contraste 0xBF ->         1 0 1 1 1 1 1 1
	 *  18. P-charge period 0xD9 ->   1 1 0 1 1 0 0 1
	 *  19. p-charge period 0x22 ->   0 0 1 0 0 0 1 0
	 *  20. 0xDB ->                   1 1 0 1 1 0 0 0
	 *  21. 0x40 ->                   0 1 0 0 0 0 0 0
	 *  22. 0x32 ->                   0 0 1 1 0 0 1 0
	 *  23. Normal display 0xA6 ->	  1 0 1 0 0 1 1 0
	 *  24. Clear   ->
	 *  25. Display ON 0xAF ->
	 *  26. Delay
	 */
	comandoOled(ptrHandlerI2C, 0xAE);
	comandoOled(ptrHandlerI2C, 0xD5);
	comandoOled(ptrHandlerI2C, 0x80);
	comandoOled(ptrHandlerI2C, 0xA8);
	comandoOled(ptrHandlerI2C, 0x3F);
	comandoOled(ptrHandlerI2C, 0xD3);
	comandoOled(ptrHandlerI2C, 0x00);
	comandoOled(ptrHandlerI2C, 0x40);
	comandoOled(ptrHandlerI2C, 0xAD);
	comandoOled(ptrHandlerI2C, 0x8B);
	comandoOled(ptrHandlerI2C, 0xA1);
	comandoOled(ptrHandlerI2C, 0xC8);
	comandoOled(ptrHandlerI2C, 0xDA);
	comandoOled(ptrHandlerI2C, 0x12);
	comandoOled(ptrHandlerI2C, 0x81);
	comandoOled(ptrHandlerI2C, 0xCF);
	comandoOled(ptrHandlerI2C, 0xD9);
	comandoOled(ptrHandlerI2C, 0x22);
	comandoOled(ptrHandlerI2C, 0xDB);
	comandoOled(ptrHandlerI2C, 0x40);
	comandoOled(ptrHandlerI2C, 0x32);
	comandoOled(ptrHandlerI2C, 0xA6);
	comandoOled(ptrHandlerI2C, 0xAF);
	delay_ms(100);
	comandoOled(ptrHandlerI2C, 0x00);
	comandoOled(ptrHandlerI2C, 0x10);
	comandoOled(ptrHandlerI2C, 0x40);

	clearScreenOLED(ptrHandlerI2C);
	setFilaInicio(ptrHandlerI2C, 0);




}

//Función para enviar un byte
void sendDataByteOled(I2C_Handler_t *ptrHandlerI2C, uint8_t data){
	//1. Generamos la condición de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de escribir
	i2c_sendSlaveAddressRW(ptrHandlerI2C, oLEDAddress, I2C_WRITE_DATA);

	//3. Enviamos el bye de control
	i2c_sendMemory_Address(ptrHandlerI2C, CONTROL_BYTE_DATO);

	//4. Enviamos varios datos
	i2c_sendDataByte(ptrHandlerI2C, data);

	//5. Generamos la condición de STOP para que el valor se detenga despúes de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);
}

//Función para enviar varios bytes
void sendDataBytesOled(I2C_Handler_t *ptrHandlerI2C, char* variosDatos){
	//1. Generamos la condición de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de escribir
	i2c_sendSlaveAddressRW(ptrHandlerI2C, oLEDAddress, I2C_WRITE_DATA);

	//3. Enviamos el bye de control
	i2c_sendMemory_Address(ptrHandlerI2C, CONTROL_BYTE_DATO);

	//4. Enviamos varios datos
	for(uint16_t i = 0; i < 8; i++){

//		i2c_sendDataByte(ptrHandlerI2C, *(variosDatos));
//		variosDatos++;
		i2c_sendDataByte(ptrHandlerI2C, variosDatos[i]);
	}
	//5. Generamos la condición de STOP para que el valor se detenga despúes de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);
}



//La oled tiene 8 páginas (de la 0 a la 7), 128 columnas y 64 filas (8 por página)
void setColumnaOled(I2C_Handler_t *ptrHandlerI2C, uint8_t columna){

	/*
	 * Las direcciones de memoria de las columnas se dividen en dos. Una parte alta
	 * y una parte baja
	 *
	 */
	uint8_t columnaLow  = 0b00000000;
	uint8_t columnaHigh = 0b00010000;

	columnaHigh = columnaHigh | (columna >> 4);
	columnaLow = columnaLow |(columna & 0b00001111);

	comandoOled(ptrHandlerI2C, columnaHigh);
	comandoOled(ptrHandlerI2C, columnaLow);

}


void setPaginaOled(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina){

	//De la hoja de datos se tiene que se debe enviar:
	// 1 0 1 1 A B C D
	//Dondde ABCD corresponde al número de la página
	uint8_t comandoPag = 0;

	comandoPag = 0b10110000 | pagina;

	comandoOled(ptrHandlerI2C, comandoPag);
}

void EscribirPaginaOled(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina, char**data){

	//Me ubico en la página correspondiente
	setPaginaOled(ptrHandlerI2C, pagina);

	//Recorro lo que quiero escribir en la página
	for(uint16_t data_i = 0; data_i < 16; data_i++){
		//El 16 es porque eso es lo que cabe en una página

//		sendDataBytesOled(ptrHandlerI2C, *(data + data_i));
//		data++;
		sendDataBytesOled(ptrHandlerI2C, data[data_i]);

	}

}

void setFilaInicio(I2C_Handler_t *ptrHandlerI2C, uint8_t StartLine){
	/*
	 * De la hoja de datos se tiene que se debe enviar:
	 * 0 1 A B C E F G
	 * Donde ABCDEFG corresponde a la línea de start
	 */
	uint8_t comandoLinea = 0;

	comandoLinea = 0b010000 | StartLine;

	comandoOled(ptrHandlerI2C, comandoLinea);
}

void clearScreenOLED(I2C_Handler_t *ptrHandlerI2C){

	//Hay que limpiar todas las páginas (renglones)
//	for(uint8_t i = 0; i < 8; i++){
//		//Me paro en la página que indica el contador
//		setPaginaOled(ptrHandlerI2C, i);

		//Cada letra es de 8x8, y en cada renglón caben 128, por lo que debo enviar
		//16 veces el caracter vacío para que limpie cada renglón:
		char* limpiarPagina[16] = {alphabet[26], alphabet[26], alphabet[26], alphabet[26], alphabet[26], alphabet[26],
									alphabet[26], alphabet[26], alphabet[26], alphabet[26], alphabet[26], alphabet[26],
									alphabet[26], alphabet[26], alphabet[26], alphabet[26]};

		//Envío todos esos 16 a cada página
		for(uint8_t pag_i = 0; pag_i < 8; pag_i++){
			EscribirPaginaOled(ptrHandlerI2C, pag_i, limpiarPagina);
		}
}




/*
 *  La oled tiene 8 páginas (de la 0 a la 7), 128 columnas y 64 filas (8 por página (renglón))
 *  Cuando se envía información para mostrar en la pantalla, se envía por columnas.
 *  Para poder formar letras completas, se tendrán que envíar todas las columnas, indicando
 *  qué filas encender.
 *
 *  un byte de información corresponde a una columna. Para formar una letra se debe enviar un arreglo
 *  de bytes considerando las columnas.
 *  El MSB de cada byte correponde al pixel inferior
 */


//Función que convierte el caracter ingresado en su respectiva posición del arreglo
char* ASCIItoChar(char caracter){
	switch (caracter){
	case('A'):{
		return *(alphabet);
		break;
	}
	case('B'):{
		return *(alphabet + 1);
		break;
	}
	case('C'):{
		return *(alphabet + 2);
		break;
	}
	case('D'):{
		return *(alphabet + 3);
		break;
	}
	case('E'):{
		return *(alphabet + 4);
		break;
	}
	case('F'):{
		return *(alphabet + 5);
		break;
	}
	case('G'):{
		return *(alphabet + 6);
		break;
	}
	case('H'):{
		return *(alphabet + 7);
		break;
	}
	case('I'):{
		return *(alphabet + 8);
		break;
	}
	case('J'):{
		return *(alphabet + 9);
		break;
	}
	case('K'):{
		return *(alphabet + 10);
		break;
	}
	case('L'):{
		return *(alphabet + 11);
		break;
	}
	case('M'):{
		return *(alphabet + 12);
		break;
	}
	case('N'):{
		return *(alphabet + 13);
		break;
	}
	case('O'):{
		return *(alphabet + 14);
		break;
	}
	case('P'):{
		return *(alphabet + 15);
		break;
	}
	case('Q'):{
		return *(alphabet + 16);
		break;
	}
	case('R'):{
		return *(alphabet + 17);
		break;
	}
	case('S'):{
		return *(alphabet + 18);
		break;
	}
	case('T'):{
		return *(alphabet + 19);
		break;
	}
	case('U'):{
		return *(alphabet + 20);
		break;
	}
	case('V'):{
		return *(alphabet + 21);
		break;
	}
	case('W'):{
		return *(alphabet + 22);
		break;
	}
	case('X'):{
		return *(alphabet + 23);
		break;
	}
	case('Y'):{
		return *(alphabet + 24);
		break;
	}
	case('Z'):{
		return *(alphabet + 25);
		break;
	}
	case(' '):{
		return *(alphabet + 26);
		break;
	}
	case('0'):{
		return *(alphabet + 27);
		break;
	}
	case('1'):{
		return *(alphabet + 28);
		break;
	}
	case('2'):{
		return *(alphabet + 29);
		break;
	}
	case('3'):{
		return *(alphabet + 30);
		break;
	}
	case('4'):{
		return *(alphabet + 31);
		break;
	}
	case('5'):{
		return *(alphabet + 32);
		break;
	}
	case('6'):{
		return *(alphabet + 33);
		break;
	}
	case('7'):{
		return *(alphabet + 34);
		break;
	}
	case('8'):{
		return *(alphabet + 35);
		break;
	}
	case('9'):{
		return *(alphabet + 36);
		break;
	}
	case(':'):{
		return *(alphabet + 37);
		break;
	}
	case('='):{
		return *(alphabet + 38);
		break;
	}
	case('$'):{
		return *(alphabet + 39);
		break;
	}

	default:{
		return *(alphabet + 26);
		break;
	}
	}
}


// Función para escribir un mensaje en una pagina
void OLED_print_msg_pag(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina, char *msgToSendOLED){

	//Me ubico en la página
	clearScreenOLED(ptrHandlerI2C);
	setPaginaOled(ptrHandlerI2C, pagina);
	setFilaInicio(ptrHandlerI2C, 0);
	comandoOled(ptrHandlerI2C, 0x02);



//	while(*(msgToSendOLED) != '\0'){
		for (uint8_t pag_i = 0; pag_i < 16; pag_i++) {
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSendOLED));
//			msgToSendOLED++;
			if(msgToSendOLED[pag_i] != '\0'){
				sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[pag_i]));
			}

		}
//	}
}

// Función para escribir un mensaje en una pagina y en una posicion particular
void OLED_print_msg_pag_inicio(I2C_Handler_t *ptrHandlerI2C, uint8_t pagina, char *msgToSendOLED, uint8_t inicio){

	//Me ubico en la página
//	clearScreenOLED(ptrHandlerI2C);
	setPaginaOled(ptrHandlerI2C, pagina);
	setFilaInicio(ptrHandlerI2C, inicio);
	comandoOled(ptrHandlerI2C, 0x02);



//	while(*(msgToSendOLED) != '\0'){
		for (uint8_t pag_i = 0; pag_i < 16; pag_i++) {
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSendOLED));
//			msgToSendOLED++;
			if(msgToSendOLED[pag_i] != '\0'){
				sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[pag_i]));
			}

		}
//	}
}
//Función para escribir un mensaje en toda la pantalla
void OLED_print_msg(I2C_Handler_t *ptrHandlerI2C, char *msgToSendOLED){

//	Limpio la pantalla
	clearScreenOLED(ptrHandlerI2C);
	comandoOled(ptrHandlerI2C, 0x02);


//	while(*(msgToSendOLED) != '\0'){

		//Me ubico en el primer renglón
		setPaginaOled(ptrHandlerI2C, 1);
		setFilaInicio(ptrHandlerI2C, 0);

		//Envío los datos del primer renglón
		for (uint8_t k = 0; k < 16; k++) {

//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSendOLED));
//			msgToSendOLED++;
			if(msgToSendOLED[k] != '\0'){
				sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[k]));
			}
			else{
				return;
			}

		}
//	}

//	while(*(msgToSendOLED) != '\0'){

		//Me ubico en el segundo renglón
		setPaginaOled(ptrHandlerI2C, 3);
		setFilaInicio(ptrHandlerI2C, 0);

		//Envío los datos del segundo renglón
		for (uint8_t k = 16; k < 32; k++) {
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSendOLED));
//			msgToSendOLED++;
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[k]));
			if(msgToSendOLED[k] != '\0'){
				sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[k]));
			}
			else{
				return;
			}
		}
//	}

//	while(*(msgToSendOLED) != '\0'){
		//Me ubico en el tercer renglón
		setPaginaOled(ptrHandlerI2C, 5);
		setFilaInicio(ptrHandlerI2C, 0);

		//Envío los datos del tercer renglón
		for (uint8_t k = 32; k < 48; k++) {
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSendOLED));
//			msgToSendOLED++;
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[k]));
			if(msgToSendOLED[k] != '\0'){
				sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[k]));
			}
			else{
				return;
			}
		}
//	}


//	while(*(msgToSendOLED) != '\0'){
		//Me ubico en el cuarto renglón
		setPaginaOled(ptrHandlerI2C, 7);
		setFilaInicio(ptrHandlerI2C, 0);

		//Envío los datos del cuarto renglón
		for (uint8_t k = 48; k < 64; k++) {
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSendOLED));
//			msgToSendOLED++;
//			sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[k]));
			if(msgToSendOLED[k] != '\0'){
				sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(msgToSendOLED[k]));
			}
			else{
				return;
			}
		}
//	}







		//	//Me ubico en el quinto renglón
		//	setPaginaOled(ptrHandlerI2C, 4);
		//	setFilaInicio(ptrHandlerI2C, 0);
		//
		//	//Envío los datos del quinto renglón
		//	for(uint8_t k = 64; k < 80; k++){
		//		sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSend));
		//		msgToSend++;
		//	}
		//
		//	//Me ubico en el sexto renglón
		//	setPaginaOled(ptrHandlerI2C, 5);
		//	setFilaInicio(ptrHandlerI2C, 0);
		//
		//	//Envío los datos del sexto renglón
		//	for(uint8_t k = 80; k < 96; k++){
		//		sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSend));
		//		msgToSend++;
		//	}
		//
		//	//Me ubico en el cuarto renglón
		//	setPaginaOled(ptrHandlerI2C, 6);
		//	setFilaInicio(ptrHandlerI2C, 0);
		//
		//	//Envío los datos del séptimo renglón
		//	for(uint8_t k = 96; k < 112; k++){
		//		sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSend));
		//		msgToSend++;
		//	}
		//
		//	//Me ubico en el cuarto renglón
		//	setPaginaOled(ptrHandlerI2C, 7);
		//	setFilaInicio(ptrHandlerI2C, 0);
		//
		//	//Envío los datos del octavo renglón
		//	for(uint8_t k = 112; k < 128; k++){
		//		sendDataBytesOled(ptrHandlerI2C, ASCIItoChar(*msgToSend));
		//		msgToSend++;
		//	}






}




