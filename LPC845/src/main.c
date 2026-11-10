#include "LPC845.h"
#include "systick.h"
#include "uart.h"
#include "wifi.h"
#include "timers.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// =========================================================================
// DEFINICIONES PARA EL TERMISTOR Y RED
// =========================================================================
#define RESISTENCIA_SERIE 10000.0   // Resistencia fija R13 de 10k en el divisor
#define RESISTENCIA_NOMINAL 10000.0 // Resistencia del NTC a 25°C
#define TEMPERATURA_NOMINAL 298.15  // 25°C convertidos a Kelvin
#define BETA_TERMISTOR 3950.0       // Constante Beta del NTC

#define RED_WIFI "Nombre_de_tu_WiFi"
#define CLAVE_WIFI "Clave_de_tu_WiFi"

// =========================================================================
// VARIABLES GLOBALES
// =========================================================================
char ip_servidor[32];
uint8_t ip_recibida = 0;
char post_request[256];
char buffer_comando_at[64];
char cuerpo_mensaje[64];

// =========================================================================
// FUNCIONES AUXILIARES
// =========================================================================

// Retardo bloqueante simple (Ajustar el multiplicador según el reloj de tu CPU)
void Retardo_ms(uint32_t ms) {
    // Si tu CPU va a 30MHz, un bucle vacío toma unos ciclos.
    // Esto es una aproximación rústica; idealmente usarías timers.h
    for (volatile uint32_t i = 0; i < (ms * 3000); i++);
}

// =========================================================================
// CONFIGURACIÓN Y LECTURA DEL ADC (CANAL 1 - PIO0_6)
// =========================================================================
void Inicializar_ADC(void) {
    // 1. Encender el reloj para el SWM y el ADC
    SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_SWM_MASK | SYSCON_SYSAHBCLKCTRL0_ADC_MASK;

    // 2. Apagar la resistencia de Pull-up/Pull-down en PIO0_6
    IOCON->PIO[6] &= ~0x18;

    // 3. Habilitar la función ADC_1 en el pin PIO0_6 (Un '0' habilita la función analógica)
    SWM0->PINENABLE0 &= ~SWM_PINENABLE0_ADC_1_MASK;

    // 4. Quitar el ADC del modo Power-Down
    SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_MASK;

    // 5. Configurar el reloj del ADC
    ADC0->CTRL = ADC_CTRL
