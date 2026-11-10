/*
 * Copyright 2022 NXP
 * NXP confidential.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms.  By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to
 * be bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#include "stdio.h"
#include "funciones.h"
#include "Infotronic.h"
#include "timers.h"
int main(void) {
	void mostrar(void);
	InicializarInfotronic();
	DisplaySegmentos(0,DSP0);
	DisplaySegmentos(0,DSP1);
	TimerStart(0, 1, SEG, mostrar);

	while(1){

	funcion();


    }
    return 0 ;
}
