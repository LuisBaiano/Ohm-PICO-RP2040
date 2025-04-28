#ifndef DISPLAY_H
#define DISPLAY_H

#include "lib/ssd1306.h"
#include "ohmmeter.h"

// === Protótipos de Funções ===
void init_oled(ssd1306_t *ssd);
void startup_screen(ssd1306_t *ssd);
void update_main_oled_display(ssd1306_t *ssd, float avg_adc, float measured_r, int standard_r, ResistorColors colors, uint16_t current_adc_mode);

#endif // DISPLAY_H