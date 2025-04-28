#include "led_matrix.h"  
#include "config.h"
#include "led_matrix.pio.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

// === Estrutura para Cores RGB  com intensidade de 0.0 a 1.0 ===
typedef struct { float r; float g; float b; } ws2812b_color_t;

// Usa as variáveis estáticas definidas no config.h via include
static PIO pio = MATRIX_PIO_INSTANCE; // Usa a instância definida em config.h
static uint sm = MATRIX_PIO_SM;       // Usa a SM definida em config.h

// === Paleta de Cores ===
static const ws2812b_color_t RESISTOR_RGB_COLORS[10] = {
    { 0.0f, 0.0f, 0.0f },       // Preto
    { 0.065f, 0.015f, 0.0075f }, // Marrom
    { 0.25f, 0.0f, 0.0f },      // Vermelho
    { 0.25f, 0.15f, 0.0f },     // Laranja
    { 0.25f, 0.25f, 0.0f },     // Amarelo
    { 0.0f, 0.25f, 0.0f },      // Verde
    { 0.0f, 0.0f, 0.25f },      // Azul
    { 0.25f, 0.1f, 0.45f },     // Violeta
    { 0.125f, 0.125f, 0.15f },  // Cinza
    { 0.25f, 0.25f, 0.25f }     // Branco
};


// === Mapeamento Físico -> Lógico das colunas da matrix de leds ===
static const int col1[5] = { 24, 15, 14,  5,  4 };
static const int col3[5] = { 22, 17, 12,  7,  2 };
static const int col5[5] = { 20, 19, 10,  9,  0 };

// === Função Auxiliar: Converte a escala RGB para Formato PIO e controla o brilho ===
static inline uint32_t color_to_pio_format(ws2812b_color_t color, float brightness) {
    float r = fmaxf(0.0f, fminf(1.0f, color.r * brightness));
    float g = fmaxf(0.0f, fminf(1.0f, color.g * brightness));
    float b = fmaxf(0.0f, fminf(1.0f, color.b * brightness));
    unsigned char R_val = (unsigned char)(r * 255.0f);
    unsigned char G_val = (unsigned char)(g * 255.0f);
    unsigned char B_val = (unsigned char)(b * 255.0f);
    return ((uint32_t)(G_val) << 24) | ((uint32_t)(R_val) << 16) | ((uint32_t)(B_val) << 8);
}
static inline uint32_t color_to_pio_format_fb(ws2812b_color_t color) {
    return color_to_pio_format(color, 1.0f);
}

// === Implementação das Funções Públicas ===

/**
 * @brief Inicializa o PIO e a state machine fixa para controlar a matriz WS2812.
 */
void init_led_matrix(void) {
    uint offset = pio_add_program(pio, &led_matrix_program); 
    led_matrix_program_init(pio, sm, offset, MATRIX_LED_PIN);
    clear_matrix(pio, sm); 
}

/**
 * @brief Apaga todos os LEDs da matriz.
 */
void clear_matrix(PIO pio_arg, uint sm_arg) { 
    uint32_t pio_black = color_to_pio_format_fb(RESISTOR_RGB_COLORS[BLACK]);
    for (int i = 0; i < 25; i++) {
        pio_sm_put_blocking(pio_arg, sm_arg, pio_black);
    }
    busy_wait_us(50);
}

/**
 * @brief Atualiza as cores da matriz para representar as faixas do resistor.
 */
void update_matrix_resistor_colors(PIO pio_arg, uint sm_arg, ResistorColors colors) {
    uint32_t pio_black = color_to_pio_format_fb(RESISTOR_RGB_COLORS[BLACK]);
    float brightness = 0.4f;
    uint32_t pixel_colors_logical[25];

    for (int i = 0; i < 25; i++) {
        pixel_colors_logical[i] = pio_black;
    }

    if (colors.valid) {
        ws2812b_color_t c1_rgb = RESISTOR_RGB_COLORS[colors.band1];
        ws2812b_color_t c2_rgb = RESISTOR_RGB_COLORS[colors.band2];
        ws2812b_color_t cM_rgb = RESISTOR_RGB_COLORS[colors.multiplier];
        uint32_t pio_c1 = color_to_pio_format(c1_rgb, brightness);
        uint32_t pio_c2 = color_to_pio_format(c2_rgb, brightness);
        uint32_t pio_cM = color_to_pio_format(cM_rgb, brightness);

        for (int i = 0; i < 5; i++) {
            int logical_idx = col1[i];
            if(logical_idx >= 0 && logical_idx < 25) { pixel_colors_logical[logical_idx] = pio_c1; }
        }
        for (int i = 0; i < 5; i++) {
            int logical_idx = col3[i];
            if(logical_idx >= 0 && logical_idx < 25) { pixel_colors_logical[logical_idx] = pio_c2; }
        }
        for (int i = 0; i < 5; i++) {
            int logical_idx = col5[i];
            if(logical_idx >= 0 && logical_idx < 25) { pixel_colors_logical[logical_idx] = pio_cM; }
        }
    }

    for (int i = 0; i < 25; i++) {
        pio_sm_put_blocking(pio_arg, sm_arg, pixel_colors_logical[i]);
    }
    busy_wait_us(50);
}