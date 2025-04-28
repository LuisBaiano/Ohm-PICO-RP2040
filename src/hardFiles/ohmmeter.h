#ifndef OHMMETER_H
#define OHMMETER_H

#include <stdbool.h>
#include <stddef.h>

// === Estruturas e Enumerações ===
typedef enum { BLACK, BROWN, RED, ORANGE, YELLOW, GREEN, BLUE, VIOLET, GREY, WHITE } color_t;

typedef struct {
    color_t band1;
    color_t band2;
    color_t multiplier;
    bool valid;
} ResistorColors;

// === Exporta as cores dos resistores ===
extern const char *COLOR_NAMES[];


// === Protótipos de Funções ===
void init_adc(void);
int find_closest_e24(float measured_r);
ResistorColors get_resistor_colors(int standard_r);

#endif // OHMMETER_H