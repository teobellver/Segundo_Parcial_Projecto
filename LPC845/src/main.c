#include "LPC845.h"

void Inicializar_ADC(void) {
    // 1. Encender el reloj para el SWM y el ADC (SYSAHBCLKCTRL0)
    SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_SWM_MASK | SYSCON_SYSAHBCLKCTRL0_ADC_MASK;

    // 2. Apagar la resistencia de Pull-up/Pull-down en PIO0_6 (Requisito para pines analógicos)
    IOCON->PIO[6] &= ~0x18; // Limpia los bits de modo

    // 3. Habilitar la función ADC_1 en el pin PIO0_6
    SWM0->PINENABLE0 &= ~SWM_PINENABLE0_ADC_1_MASK;

    // 4. Quitar el ADC del modo Power-Down
    SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_MASK;

    // 5. Configurar el reloj del ADC (Divisor)
    // Se asume un reloj de sistema de 30MHz. El ADC requiere <= 72MHz.
    ADC0->CTRL = ADC_CTRL_CLKDIV(0);

    // 6. Calibración del ADC
    ADC0->CTRL |= ADC_CTRL_CALMODE(1);
    while (ADC0->CTRL & ADC_CTRL_CALMODE_MASK); // Esperar a que termine la calibración

    // 7. Configurar la Secuencia A (SEQA) para leer el Canal 1
    // Borramos los canales anteriores y seleccionamos el canal 1 (bit 1)
    ADC0->SEQ_CTRL[0] &= ~ADC_SEQ_CTRL_CHANNELS_MASK;
    ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_CHANNELS(1 << 1);

    // 8. Habilitar la Secuencia A
    ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_SEQ_ENA(1);
}

uint16_t Leer_ADC(void) {
    // Disparar la conversión de la Secuencia A por software
    ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_START(1);

    // Esperar a que la conversión termine (monitoreando el bit DATAVALID del canal 1)
    while (!(ADC0->DAT[1] & ADC_DAT_DATAVALID_MASK));

    // Extraer el resultado de 12 bits (bits 4 a 15) usando los macros de tu header
    uint16_t resultado = (ADC0->DAT[1] & ADC_DAT_RESULT_MASK) >> ADC_DAT_RESULT_SHIFT;

    return resultado;
}
