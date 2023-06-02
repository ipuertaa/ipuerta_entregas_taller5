/*
 * OledDriver.c
 *
 *  Created on: Jun 2, 2023
 *      Author: isabel
 */


#include "OledDriver.h"
#include <stdint.h>
#include "I2CDriver.h"


/*
 *  			INFORMACIÓN GENERAL
 *
 * Dirección del esclavo: 0x78
 *
 *
 * Antes de enviar cualquier instrucción se debe enviar un bit de control:
 * 	C D 0 0 0 0 0 0
 *
 *
 */




