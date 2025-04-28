#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "hardware/pio.h"
#include "ohmmeter.h"

// === Protótipos de Funções ===
void init_led_matrix(void);
void update_matrix_resistor_colors(PIO pio, uint sm, ResistorColors colors);
void clear_matrix(PIO pio, uint sm);

#endif // LED_MATRIX_H