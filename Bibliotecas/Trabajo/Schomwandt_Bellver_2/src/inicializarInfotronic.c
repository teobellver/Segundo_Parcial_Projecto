/*******************************************************************************************************************************//**
 *
 * @file		inicializar.cpp
 * @brief		Descripcion del modulo
 * @date		5 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/

#include "Infotronic.h"

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/**********************************************************************************
 *** OBJETOS GLOBALES PUBLICOS
 **********************************************************************************/

void InicializarInfotronic ( void )
{
	Inicializar_PLL( );
	Inicializar_GPIO( );
	Inicializar_Teclado( );
	Inicializar_I4017( );
	Inicializar_I4511( );
	Inicializar_DiplaySegmentos( );
	Inicializar_ADC( );
	Inicializar_Inputs( );
	Inicializar_Outputs( );
	Inicializar_SysTick( 2 );
	Inicializar_Lcd( );
	Inicializar_Timers ( );
	Inicializar_COM(
				COM_0 ,
				9600 ,				// baudios
				ocho_bits ,			// bits de datos
				NoParidad 			// tipo de paridad
		);

	Inicializar_WIFI();
	Inicializar_DAC( );
}
