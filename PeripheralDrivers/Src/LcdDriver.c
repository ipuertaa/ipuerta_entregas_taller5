/*
 * LcdDriver.c
 *
 *  Created on: May 27, 2023
 *      Author: isabel
 *
 */

#include "LcdDriver.h"
#include "SysTickDriver.h"
#include "I2CDriver.h"
uint8_t j = 0;
uint8_t k = 0;



/*
 * 									INFORMACIÓN GENERAL SOBRE LA PANTALLA
 * - Bit RS: selección de registros
 * 			0: Registro de instrucción
 * 			1: Registro de datos
 *
 * - Bit R/W: seleccionar si se va a leer o a escribir
 * 			0: escribir
 * 			1: leer
 *
 * - Bit E: habilita la lectura o escritura de datos
 *
 * - Hay 8 bits de datos. Deben ser partidos en 4 bits. primero enviar la parte
 * alta y luego la parte baja de esos 8 bits.
 *
 * 										SECUENCIA DE FUNCIONES
 *
 * - Secuencia de inicialización
 * - Configuración de la pantalla. (activar el display, cursor)
 *
 *
 */

// Función para dividir el dato en la parte baja y parte alta
void particion(I2C_Handler_t *ptrHandlerI2C, uint8_t tipo, uint8_t datoOcmd){
	//Enviar los MSB
	manejoDatos_LCD(ptrHandlerI2C, tipo, (datoOcmd >> 4));
	//Enviar los LSB
	manejoDatos_LCD(ptrHandlerI2C, tipo, (datoOcmd & 0x0F));
}




//Función para el manejo de los datos, ya sean como tipo comando o tipo dato
void manejoDatos_LCD(I2C_Handler_t *ptrHandlerI2C, uint8_t tipo, uint8_t datoOcmd){
	//Para escribir comandos debo estar en el registro de instrucción RS = 0
	//Para escribir datos debo estar en el registro de datos RS = 1;

	uint8_t auxDato = 0;
	//Se debe mover 4 veces para garantizar que el dato quede en los BITS superiores
	//ya que la configuración de la pantalla es a 4 bits
	auxDato = (datoOcmd << 4);

	if(tipo == DATO){
		//Estamos en el registro de datos, por lo que, el bit RS debe quedar en 1
		auxDato = auxDato | RS;
	}
	else {
		//Estamos en el registro de configuración, por lo que el bit RS debe quedar en cero

		auxDato = auxDato & ~(RS);
	}

	//Se llama a la función, para que escriba a la LCD la información.
	//Primero se activa el bit E (enable) para que permita que los datos lleguen al registro correspondiente
	escribir_LCD(ptrHandlerI2C, (auxDato | E));
	//Se vuelve a desactivar el enable
	escribir_LCD(ptrHandlerI2C, (auxDato & ~E));

}

void escribir_LCD(I2C_Handler_t *ptrHandlerI2C, char dato){

	// Iniciar la comunicación I2C
	i2c_startTransaction(ptrHandlerI2C);

	//Enviar la dirección del esclavo y el modo escritura
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//Enviamos lo que deseamos escribir
	i2c_sendDataByte(ptrHandlerI2C, (dato|LED));

	//Generamos la condición de stop
	i2c_stopTransaction(ptrHandlerI2C);
}



void Init_LCD(I2C_Handler_t *ptrHandlerI2C){

	//Verifico que la comunicación se establezca, escribiendo un 0x00
	i2c_startTransaction(ptrHandlerI2C);
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);
	i2c_sendDataByte(ptrHandlerI2C, 0x00);
	i2c_stopTransaction(ptrHandlerI2C);

	//Esperar más de 40 ms
	delay_ms(50);

	//Enviar un comando de SET
	manejoDatos_LCD(ptrHandlerI2C, COMANDO, SET);

	// Esperar mas de 4.1 ms
	delay_ms(5);

	//Enviar un comando de SET
	manejoDatos_LCD(ptrHandlerI2C, COMANDO, SET);

	//Esperar mas de 100us
	delay_ms(5);

	//Enviar un comando de SET
	manejoDatos_LCD(ptrHandlerI2C, COMANDO, SET);
	delay_ms(5);

	//Envío el comando para que el cursor esté en home
	manejoDatos_LCD(ptrHandlerI2C, COMANDO, 0x02);

	//Configuro el display
	//Modo de 4 bits, display con 2 filas, fuente: matriz 5x8
	particion(ptrHandlerI2C, COMANDO, 0x28);
	delay_ms(5);

	//Apago el display
	particion(ptrHandlerI2C, COMANDO, 0x08);
	delay_ms(30);

	//Limpiar display
	particion(ptrHandlerI2C, COMANDO, 0x01);
	delay_ms(5);

	//Configurando el modo activación.
	//Cursor se traslada a la derecha, lo escrito permanece inmovil
	particion(ptrHandlerI2C, COMANDO, 0x06);
	delay_ms(5);

	//Configuración del display ON/OFF
	//Se enciende el display, parpadeo del cursor activo.
	particion(ptrHandlerI2C, COMANDO, 0x0F);
	delay_ms(5);
}

// Función para enviar un nuevo comando a la LCD
void comandos_LCD(I2C_Handler_t *ptrHandlerI2C, uint8_t comando){
	particion(ptrHandlerI2C, COMANDO, comando);
	delay_ms(10);
}

// Función para enviar un nuevo dato a la LCD
void nuevoDato_LCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dato){
	particion(ptrHandlerI2C, DATO, dato);
	delay_ms(10);
}

void LCD_XY(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y){
	switch (y){
	case 1: {
		comandos_LCD(ptrHandlerI2C, (0x80 + (x-1)));
		break;
	}
	case 2: {
		comandos_LCD(ptrHandlerI2C, (0xC0 + (x-1)));
		break;
	}
	case 3: {
		comandos_LCD(ptrHandlerI2C, (0x94 + (x-1)));
		break;
	}
	case 4: {
		comandos_LCD(ptrHandlerI2C, (0xD4 + (x-1)));
		break;
	}
	}
	delay_ms(50);
}

void LCD_dato_XY_(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y, uint8_t caracter){
	LCD_XY(ptrHandlerI2C, x, y);
	particion(ptrHandlerI2C, DATO, caracter);
}

void LCD_out_Msg(I2C_Handler_t *ptrHandlerI2C, unsigned char Msg[]){
	while(Msg[j] != '\0'){
		nuevoDato_LCD(ptrHandlerI2C, Msg[j]);
		j++;
	}

}

void LCD_Out_Msg_XY(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y, unsigned char Msg[]){
	LCD_XY(ptrHandlerI2C, x, y);
	while(Msg[k] != '\0'){
		nuevoDato_LCD(ptrHandlerI2C, Msg[j]);
	}
}










