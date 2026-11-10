/*
 * dac.h
 *
 *  Created on: Apr 12, 2023
 *      Author: fedebua
 */

#ifndef DAC_H_
#define DAC_H_

#include "LPC845.h"

#define 	Inicializar_DAC( )	InicializarDAC( 1 )

extern bool enabled;
extern uint8_t dac_n;
extern DAC_Type* dac;

int8_t enable( uint8_t dac_n );
int8_t disable( uint8_t dac_n );
void set_value(uint16_t value);
void config_dac0_power(bool enable);
void config_dac1_power(bool enable);
void config_dac0_clock(bool enable);
void config_dac1_clock(bool enable);
void config_dac0_swm(bool enable);
void config_dac1_swm(bool enable);
void config_dac0_pinmode(bool enable);
void config_dac1_pinmode(bool enable);
int8_t TensionOut( float v );
int8_t SetCuentas_DAC(uint16_t cuentas);
void InicializarDAC( uint8_t n_dac );
void disable_dac0(void);
void disable_dac1(void);


#endif /* DAC_H_ */
