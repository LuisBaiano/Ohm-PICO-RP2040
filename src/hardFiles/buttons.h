#ifndef BUTTONS_H
#define BUTTONS_H

#include "hardware/gpio.h"
#include <stdbool.h> // Para bool

// === Variável que sinaliza que o botão A foi pressionado ===
extern volatile bool g_button_a_pressed_flag;

// === Protótipos de Funções ===
void init_buttons_gpio(void);
void init_bootsel_button(void);
void button_irq_callback(uint gpio, uint32_t events);

#endif // BUTTONS_H