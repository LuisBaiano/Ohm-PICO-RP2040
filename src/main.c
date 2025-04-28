#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/adc.h" // Leitura de ADC

// --- Headers dos módulos ---
#include "config.h"
#include "hardFiles/ohmmeter.h"
#include "hardFiles/display.h"
#include "hardFiles/buttons.h"
#include "hardFiles/led_matrix.h"

// --- Variáveis Globais ---
/**
 * @brief Instância do display OLED.
 */
static ssd1306_t ssd;

/**
 * @brief Valor máximo atual do ADC, alternando entre USB e Bateria.
 */
static uint16_t current_adc_max_value = ADC_MAX_USB;

/**
 * @brief Função principal. 
 * Inicializa periféricos, gerencia leitura do ADC, calcula resistência, 
 * determina padrão E24, atualiza display OLED e matriz de LEDs.
 */

int main() {
    stdio_init_all();
    sleep_ms(1000);
    printf("Projeto Ohmimetro sendo inicializado!");
    // --- Inicialização dos módulos ---
    init_buttons_gpio();    // Inicializa botão A
    init_bootsel_button();  // Inicializa botão B (bootsel)
    init_adc();             // Inicializa ADC
    init_oled(&ssd);        // Inicializa OLED
    init_led_matrix();      // Inicializa Matriz de LEDs

    // Exibe tela inicial
    startup_screen(&ssd);

    // --- Loop Principal ---
    while (true) {
        if (g_button_a_pressed_flag) {
            /**
             * @brief Alterna o modo do ADC entre USB e Bateria ao pressionar o botão A.
             */
            current_adc_max_value = (current_adc_max_value == ADC_MAX_USB) ? ADC_MAX_BATTERY : ADC_MAX_USB;
            g_button_a_pressed_flag = false;

            printf("Modo ADC alterado para: %d (%s)\n",
                current_adc_max_value,
                (current_adc_max_value == ADC_MAX_USB) ? "USB" : "BAT");
        }

        // Leitura e calculo de média do ADC
        adc_select_input(ADC_CHANNEL);
        uint32_t adc_sum = 0;
        for (int i = 0; i < ADC_SAMPLES; i++) {
            adc_sum += adc_read();
        }
        float avg_adc = (float)adc_sum / ADC_SAMPLES;

        // Cálculo da resistência medida
        float r_medido;
        if (avg_adc >= current_adc_max_value - 15) r_medido = -1.0f;
        else if (avg_adc < 15) r_medido = 0.0f;
        else r_medido = (R_DIVISOR * avg_adc) / (current_adc_max_value - avg_adc);

        // Encontra o valor mais próximo na série E24 para obter as cores
        int r_padrao = -1;
        ResistorColors cores = {.valid = false};
        if (r_medido > 0) {
            r_padrao = find_closest_e24(r_medido);
            if (r_padrao > 0) { cores = get_resistor_colors(r_padrao); }
        }

        // Atualizar display OLED e matriz de LEDs
        update_main_oled_display(&ssd, avg_adc, r_medido, r_padrao, cores, current_adc_max_value);
        update_matrix_resistor_colors(MATRIX_PIO_INSTANCE, MATRIX_PIO_SM, cores);

        // Aguarda próximo ciclo
        sleep_ms(500);
    }
    return 0;
}
