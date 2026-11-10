/*
 * seg4511.h
 *
 *  Created on: 30 oct. 2025
 *      Author: Marcelo Trujillo
 */

#ifndef MODULOS_SEG4511_H_
#define MODULOS_SEG4511_H_

#include "LPC845.h"
#include "Gpio.h"

void Inicializar_I4511 ( void );
void I4511_Clear ( void );
void I4511_Set( uint16_t segmentos  );

#endif /* MODULOS_SEG4511_H_ */
