/*
 * USARTxDriver.c
 *
 *  Created on: Apr 26, 2023
 *      Author: isabel
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

uint8_t auxRxData = 0;
char auxTxData = 0;
uint8_t i = 0;
char *auxMsgToSend;	//NO lo tengo que inicializar
uint8_t flagChar = 0;
uint8_t flagMsg = 0;


/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		RCC->APB2ENR |= (RCC_APB2ENR_USART1EN);
	}

    /* 1.2 Configuramos el USART2 */
	else if(ptrUsartHandler->ptrUSARTx == USART2){
		RCC->APB1ENR |= (RCC_APB1ENR_USART2EN);
	}

    /* 1.3 Configuramos el USART2 */
	else if(ptrUsartHandler->ptrUSARTx == USART6){
		RCC->APB2ENR |= (RCC_APB2ENR_USART6EN);
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_PCE);

		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada

			ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_PS);


		}else{
			// la paridad seleccionada es ODD, y cargamos esta configuracion

			ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_PS);
		}
	}else{
		// Si llegamos aca, es porque no deseamos tener el parity-check

		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_PCE);
	}

	// 2.3 Configuramos el tamaño del dato
    if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT){

    	ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_M);
    }

    else if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_9BIT){
    	ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_M);
    }


	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		// Debemoscargar el valor 0b00 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);	//Limpiar
		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemoscargar el valor 0b01 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);	//Limpiar
		ptrUsartHandler->ptrUSARTx->CR2 |= (USART_CR2_STOP_0);
		break;
	}
	case USART_STOPBIT_2: {
		// Debemoscargar el valor 0b10 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);	//Limpiar
		ptrUsartHandler->ptrUSARTx->CR2 |= (USART_CR2_STOP_1);
		break;
	}
	case USART_STOPBIT_1_5: {
		// Debemoscargar el valor 0b11 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);	//Limpiar
		ptrUsartHandler->ptrUSARTx->CR2 |= (USART_CR2_STOP);
		break;
	}
	default: {
		// En el caso por defecto seleccionamos 1 bit de parada
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);
		break;
	}
	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;
	if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
		// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
		// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
		// Valor a cargar 0x0683
		// Configurando el Baudrate generator para una velocidad de 9600bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
	}

	else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
		// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
		// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
		// Valor a cargar 0x341
		ptrUsartHandler->ptrUSARTx->BRR = 0x341;
	}

	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
		// El valor a cargar es 8.6875 -> Mantiza = 8, fraction = 0.6875
		// Mantiza = 8 = 0x8, fraction = 16*0.6875 = 11 = B
		// Valor a cargar 0x8A
		ptrUsartHandler->ptrUSARTx->BRR = 0x8B;
	}

	//Configuración de los valores de BRR si el micro se encuentra a Frec = 80MHz, overr = 0;
	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_80MHz_9600){
		//El valor a cargar es 520.83333. ->Mantiza = 520, Fraction = 0.83333
		//Mantiza = 520 = 0x208, fraction = 16*0.83333 = 13 = D
		//Valor a cargar = 0x208D
		ptrUsartHandler->ptrUSARTx->BRR = 0x208D;
	}

	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_80MHz_19200){
		//El valor a cargar es 260.41666 -> Mantiza = 260, Fraction = 0.41666
		//Mantiza = 260 = 0x104, Fraction = 16*0.41666 = 6 = 0x6
		//Valor a cargar = 0x1046
		ptrUsartHandler->ptrUSARTx->BRR = 0x1046;
	}

	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_80MHz_115200){
		//El valor a cargar es 43.40277 -> Mantiza = 43, Fraction = 0.40277
		//Mantiza = 43 = 0x2B, fraction = 16*0.40277 = 6 = 0x6
		//Valor a cargar = 0x02B6
		ptrUsartHandler->ptrUSARTx->BRR = 0x02B6;
	}

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar. Bit TE en el CR1
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_TE);
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_RE);
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_TE); 		// Transmisión
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_RE);		// Recepción
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TE);
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_RE);
		break;
	}

	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_RE);
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TE);
		break;
	}
	}

	//2.6.1 Configuración de las interrupciones
	switch(ptrUsartHandler->USART_Config.USART_enableIntRX){

	case USART_RX_INTERRUPT_ENABLE:
	{
		//Desactivamos las interrupciones globales
		__disable_irq();

		//Activar la interrupción del canal que se está configurando
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_RXNEIE);
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RXNEIE;


		//Activar el canal del NVIC para que se lea la interrupción
		if(ptrUsartHandler->ptrUSARTx == USART1){
			//Activar el NVIC para la interrupción del USART1
			__NVIC_EnableIRQ(USART1_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART2){
			//Activar el NVIC para la interrupción del USART1
			__NVIC_EnableIRQ(USART2_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART6){
			//Activar el NVIC para la interrupción del USART1
			__NVIC_EnableIRQ(USART6_IRQn);
		}
		else{
			__NOP();
		}

		//Activamos las interrupciones globales
		__enable_irq();
		break;
	}

	case USART_RX_INTERRUPT_DISABLE:
	{
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_RXNEIE);
		break;
	}

	case USART_TX_INTERRUPT_ENABLE:
	{
		//Desactivamos las interrupciones globales
		__disable_irq();

		//Activar el canal del NVIC para que se lea la interrupción
		if(ptrUsartHandler->ptrUSARTx == USART1){
			//Activar el NVIC para la interrupción del USART1
			__NVIC_EnableIRQ(USART1_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART2){
			//Activar el NVIC para la interrupción del USART1
			__NVIC_EnableIRQ(USART2_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART6){
			//Activar el NVIC para la interrupción del USART1
			__NVIC_EnableIRQ(USART6_IRQn);
		}
		else{
			__NOP();
		}

		//Activamos las interrupciones globales
		__enable_irq();
		break;
	}

	case USART_TX_INTERRUPT_DISABLE:
	{
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TXEIE);
		break;
	}

	default:
	{
		//Se desactivan todas las interrupciones
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TXEIE);
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_RXNEIE);
		break;
	}
	}

	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_UE);
	}
}	//Fin Usart_Config





//

/* funcion para escribir un solo char */
void writeChar(USART_Handler_t *ptrUsartHandler, char dataToSend){
//	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
//		__NOP();
//	}
	auxTxData = dataToSend;
	flagChar = 1;
	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;



}

void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend){

	//Almaceno lo que quiero enviar
	auxMsgToSend = msgToSend;
	flagChar = 2;
	i=0;
	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;

}

/* Función para la lectura del caracter que llega por la interface serial*/
uint8_t getRxData(void){
	return auxRxData;
}

/*
 * El TXE siempre está activo a menos que haya un dato por transmitir.
 * Debo tener una función mediante la cual pueda activar las
 * interrupciones de la transmisión solo cuando quiera transmitir, para
 * que de una salte la interrupción.
 * Además, debo tener otra función para desactivar las interrupciones
 * para que se pueda salir de la interrupción cuando el dato se envíe.
 */

void TxInterrupt_Enable(USART_Handler_t *ptrUsartHandler){
	//Activar las interrupciones del canal que se está configurando
	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;
//	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TCIE;
}

void TxInterrupt_Disable(USART_Handler_t *ptrUsartHandler){
	//Desactivar las interrupciones del canal que se está configurando
	ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TXEIE);
//	ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TCIE);

}




//Handler de la interrupción del USART

void USART1_IRQHandler(void){
	//Evaluamos si la interrupción que se dió es por RX
	if(USART1->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1->DR;
		usart1Rx_Callback();
	}
	else if(USART1->SR & USART_SR_TXE){
		if(flagChar == 1){
			USART1->DR = auxTxData;
			flagChar = 0;
			//Desactivo la interrupción
			USART1->CR1 &= ~(USART_CR1_TXEIE);
		}
		else if(flagChar == 2){
			if(auxMsgToSend[i] != '\0'){
				USART1->DR = auxMsgToSend[i];
				i++;
			}
			else{
				//Desactivo la interrupción
				USART1->CR1 &= ~(USART_CR1_TXEIE);
			}
		}
	}

}

void USART2_IRQHandler(void){
	if(USART2->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2->DR;
		usart2Rx_Callback();
	}
	else if(USART2->SR & USART_SR_TXE){

		if(flagChar == 1){ //WriteChar
			USART2->DR =auxTxData;
			flagChar = 0;
			//Desactivo la interrupción
			USART2->CR1 &= ~(USART_CR1_TXEIE);
		}
		else if(flagChar == 2){//WriteMsg
			if(auxMsgToSend[i] != '\0'){
				USART2->DR = auxMsgToSend[i];
				i++;
			}
			else{
				//Desactivo la interrupción
				USART2->CR1 &= ~(USART_CR1_TXEIE);
			}
		}
	}
}

void USART6_IRQHandler(void){
	if(USART6->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6->DR;
		usart6Rx_Callback();
	}
	else if(USART6->SR & USART_SR_TXE){
		USART6->DR = auxTxData;

		//Desactivo la interrupción
		USART6->CR1 &= ~(USART_CR1_TXEIE);
	}
}

__attribute__ ((weak)) void usart1Rx_Callback(void){
	__NOP();
}

__attribute__ ((weak)) void usart2Rx_Callback(void){
	__NOP();
}

__attribute__ ((weak)) void usart6Rx_Callback(void){
	__NOP();
}

__attribute__ ((weak)) void usart1Tx_Callback(void){
	__NOP();
}

__attribute__ ((weak)) void usart2Tx_Callback(void){
	__NOP();
}

__attribute__ ((weak)) void usart6Tx_Callback(void){
	__NOP();
}





