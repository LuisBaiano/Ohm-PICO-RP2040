#include "ohmmeter.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// === Constantes ===

/**
 * @brief Valores padrão da série E24 usados para referência de resistores.
 */
const int E24_VALUES[] = {
    510, 560, 620, 680, 750, 820, 910, 1000, 1100, 1200, 1300, 1500, 1600, 1800, 2000,
    2200, 2400, 2700, 3000, 3300, 3600, 3900, 4300, 4700, 5100, 5600, 6200, 6800, 7500,
    8200, 9100, 10000, 11000, 12000, 13000, 15000, 16000, 18000, 20000, 22000, 24000,
    27000, 30000, 33000, 36000, 39000, 43000, 47000, 51000, 56000, 62000, 68000, 75000,
    82000, 91000, 100000
};

/**
 * @brief Número total de valores na série E24.
 */
const int NUM_E24_VALUES = sizeof(E24_VALUES) / sizeof(E24_VALUES[0]);

/**
 * @brief Nomes das cores correspondentes aos dígitos de resistores.
 */
const char *COLOR_NAMES[] = { "Preto", "Marrom", "Vermelho", "Laranja", "Amarelo", "Verde", "Azul", "Violeta", "Cinza", "Branco" };

// === Implementação das Funções ===

/**
 * @brief Inicializa o ADC e configura o pino e canal utilizados.
 */
void init_adc(void) {
    adc_init();
    adc_gpio_init(ADC_GPIO_PIN);
    adc_select_input(ADC_CHANNEL);
}

/**
 * @brief Encontra o valor mais próximo da série E24 para uma resistência medida.
 * 
 * @param measured_r Resistência medida.
 * @return int Valor E24 mais próximo ou -1 se fora da faixa válida.
 */
int find_closest_e24(float measured_r) {
    if (measured_r < E24_VALUES[0] * 0.90f) return -1;
    if (measured_r > E24_VALUES[NUM_E24_VALUES - 1] * 1.10f) return -1;
    int closest_val = E24_VALUES[0];
    float min_diff = fabsf(measured_r - closest_val);
    for (int i = 1; i < NUM_E24_VALUES; i++) {
        float diff = fabsf(measured_r - E24_VALUES[i]);
        if (diff < min_diff) {
            min_diff = diff;
            closest_val = E24_VALUES[i];
        }
    }
    return closest_val;
}

/**
 * @brief Converte um valor de resistência padrão em suas cores correspondentes.
 * 
 * @param standard_r Valor da resistência padrão.
 * @return ResistorColors Estrutura contendo as bandas de cor e validade da conversão.
 */
ResistorColors get_resistor_colors(int standard_r) {
    ResistorColors colors = {.valid = false};
    if (standard_r <= 0) return colors;
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", standard_r);
    int len = strlen(buffer);
    if (len < 2) { return colors; }
    int d1 = buffer[0] - '0';
    int d2 = buffer[1] - '0';
    int mult = len - 2;
    if (d1 < 0 || d2 < 0 || mult < 0 || d1 > 9 || d2 > 9 || mult > 9) { return colors; }
    colors.band1 = (color_t)d1;
    colors.band2 = (color_t)d2;
    colors.multiplier = (color_t)mult;
    colors.valid = true;
    return colors;
}
