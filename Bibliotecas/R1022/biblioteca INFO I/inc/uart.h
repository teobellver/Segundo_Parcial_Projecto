/*******************************************************************************************************************************//**
 *
 * @file		uart.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		5 oct. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/

#ifndef I1_USART_UART_H_
#define I1_USART_UART_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "LPC845.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#define		COM_0		0
#define		COM_1		1

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/
typedef enum  { NoParidad , par = 2, impar} paridad_t;
typedef enum  { siete_bits , ocho_bits } bits_de_datos_t;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPOS
 **********************************************************************************************************************************/

//void Inicializar_UART( uint8_t portTx , uint8_t pinTx , uint8_t portRx , uint8_t pinRx ,
//		USART_Type * usart,uint32_t baudrate, bits_de_datos BitsDeDatos, paridad_t paridad
//		);
//void Inicializar_UART0(
//		uint32_t baudrate,			// velocidad de transmisión
//		uint8_t BitsDeDatos,		// Cantidad de bits de datos
//		uint8_t paridad 			// Tipo de paridad
//					);
//void Inicializar_UART1(
//		uint32_t baudrate,			// velocidad de transmisión
//		uint8_t BitsDeDatos,		// Cantidad de bits de datos
//		uint8_t paridad 			// Tipo de paridad
//					);
void Inicializar_COM(
		uint8_t  com ,				// uart seleccionada
		uint32_t baudrate,			// velocidad de transmisión
		uint8_t BitsDeDatos,		// Cantidad de bits de datos
		uint8_t paridad 			// Tipo de paridad
					);
void Transmit ( void * msg , uint32_t n , uint8_t com );
void* Receive ( void * msg , uint32_t n , uint8_t com );
void UART_IRQHandler (void);
uint8_t popRx (uint8_t * dato , uint8_t com );
//void pushRx ( uint8_t dato , uint8_t com );
//void pushTx ( uint8_t dato , uint8_t com );
//uint8_t popTx (uint8_t * dato , uint8_t com );
//
void Tx_EnableInterupt  (  uint8_t com );
void Tx_DisableInterupt  (  uint8_t com );
void UART0_IRQ ( void );
void UART1_IRQ ( void );
void UART2_IRQ ( void );
void UART3_IRQ ( void );
void UART4_IRQ ( void );

#endif /* I1_USART_UART_H_ */
