#include "display.h"
#include "config.h"
#include "lib/font.h"
#include "ohmmeter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define RES_X_START 20
#define RES_Y_CENTER 48
#define RES_BODY_W 50
#define RES_BODY_H 12
#define RES_LEAD_LEN 12
#define RES_BAND_W 5
#define RES_BAND_SPACE 4
#define RES_TEXT_Y (RES_Y_CENTER + RES_BODY_H / 2 + 2)

//Desenha um resistor no display OLED
static void draw_resistor(ssd1306_t *ssd) {
    uint8_t body_x = RES_X_START + RES_LEAD_LEN;
    uint8_t body_y = RES_Y_CENTER - (RES_BODY_H / 2);
    uint8_t body_end_x = body_x + RES_BODY_W;
    uint8_t lead_y = RES_Y_CENTER;
    ssd1306_line(ssd, RES_X_START, lead_y, body_x, lead_y, true);
    ssd1306_line(ssd, body_end_x, lead_y, body_end_x + RES_LEAD_LEN, lead_y, true);
    ssd1306_rect(ssd, body_y, body_x, RES_BODY_W, RES_BODY_H, true, false);
    uint8_t band_start_x = body_x + RES_BAND_SPACE * 2;
    uint8_t band_draw_y = body_y + 1;
    uint8_t band_draw_h = RES_BODY_H - 2;
    ssd1306_rect(ssd, band_draw_y, band_start_x, RES_BAND_W, band_draw_h, true, true);
    ssd1306_rect(ssd, band_draw_y, band_start_x + RES_BAND_W + RES_BAND_SPACE, RES_BAND_W, band_draw_h, true, true);
    ssd1306_rect(ssd, band_draw_y, band_start_x + 2*(RES_BAND_W + RES_BAND_SPACE), RES_BAND_W, band_draw_h, true, true);
}

// Inicializa o display OLED
void init_oled(ssd1306_t *ssd) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    ssd1306_init(ssd, WIDTH, HEIGHT, false, OLED_I2C_ADDR, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
    printf("Display OLED inicializado.\n");
}

//Exibe uma tela inicial com o nome do projeto e um resistor
void startup_screen(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    uint8_t center_x_approx = ssd->width / 2;
    uint8_t start_y = 8;
    uint8_t line_height = 10;
    const char *line1 = "EMBARCATECH";
    const char *line2 = "PROJETO";
    const char *line3 = "OHMIMETRO";
    ssd1306_draw_string(ssd, line1, center_x_approx - (strlen(line1)*8)/2, start_y);
    ssd1306_draw_string(ssd, line2, center_x_approx - (strlen(line2)*8)/2, start_y + line_height);
    ssd1306_draw_string(ssd, line3, center_x_approx - (strlen(line3)*8)/2, start_y + 2*line_height);
    draw_resistor(ssd);
    ssd1306_send_data(ssd);
    sleep_ms(2100);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

void update_main_oled_display(ssd1306_t *ssd, float avg_adc, float measured_r, int standard_r, ResistorColors colors, uint16_t current_adc_mode) {
    char str_adc[6];
    char str_measured_val[15];
    char str_standard_val[15]; 
    const char *str_c1 = "---N/D---";
    const char *str_c2 = "---N/D---";
    const char *str_cM = "---N/D---";
    char str_mode[4] = "   ";

    // --- Formatação ---
    snprintf(str_adc, sizeof(str_adc), "%4.0f", avg_adc < 0 ? 0 : avg_adc);

    if (measured_r < 0) { snprintf(str_measured_val, sizeof(str_measured_val), "Aberto"); }
    else if (measured_r < 1.0) { snprintf(str_measured_val, sizeof(str_measured_val), " Curto"); }
    else {
        // Formata como float sem casas decimais
        snprintf(str_measured_val, sizeof(str_measured_val), "%.0f", measured_r);
    }

    // Formata o Valor Comercial (E24)
    if (standard_r > 0) {
        snprintf(str_standard_val, sizeof(str_standard_val), "%d", standard_r); // Adiciona unidade para clareza
    } else if (measured_r > 1.0) {
        snprintf(str_standard_val, sizeof(str_standard_val), "N/D E24");
    } else {
        snprintf(str_standard_val, sizeof(str_standard_val), "------");
    }

    if (colors.valid) {
        str_c1 = COLOR_NAMES[colors.band1];
        str_c2 = COLOR_NAMES[colors.band2];
        str_cM = COLOR_NAMES[colors.multiplier];
    }
    //Mostra qual o modo de controle de valor do ADc está ativo
    if (current_adc_mode == ADC_MAX_USB) { snprintf(str_mode, sizeof(str_mode), "USB"); }
    else if (current_adc_mode == ADC_MAX_BATTERY) { snprintf(str_mode, sizeof(str_mode), "BAT"); }

    // --- Desenho ---
    ssd1306_fill(ssd, false);
    uint8_t top_section_h = 28;
    uint8_t bottom_section_y = top_section_h + 2;
    uint8_t vertical_line_x = 38;

    // Cores com Rótulos (Topo)
    uint8_t label_x_colors = 2;
    uint8_t value_x_colors = label_x_colors + 8*7;
    uint8_t color_text_y = 2;
    uint8_t color_line_height = 9;
    ssd1306_draw_string(ssd, "Faixa:", label_x_colors, color_text_y);
    ssd1306_draw_string(ssd, (char *)str_c1, value_x_colors, color_text_y);
    ssd1306_draw_string(ssd, "Faixa:", label_x_colors, color_text_y + color_line_height);
    ssd1306_draw_string(ssd, (char *)str_c2, value_x_colors, color_text_y + color_line_height);
    ssd1306_draw_string(ssd, "Multi:", label_x_colors, color_text_y + 2 * color_line_height);
    ssd1306_draw_string(ssd, (char *)str_cM, value_x_colors, color_text_y + 2 * color_line_height);


    // Linhas Divisórias
    ssd1306_line(ssd, 0, top_section_h, ssd->width - 1, top_section_h, true);
    ssd1306_line(ssd, vertical_line_x, bottom_section_y - 1, vertical_line_x, ssd->height - 1, true);

    // ADC (Esquerda)
    uint8_t label_y = bottom_section_y + 2;
    uint8_t value_y1 = label_y + 10;
    uint8_t value_y2 = value_y1 + 10;
    ssd1306_draw_string(ssd, "ADC", 5, label_y); 
    ssd1306_draw_string(ssd, str_adc, 5, value_y1);
    ssd1306_draw_string(ssd, str_mode, 5, value_y2); // USB/BAT abaixo do valor ADC

    // Resistência (Direita)
    uint8_t res_label_x = vertical_line_x + 5;
    uint8_t res_value_x = res_label_x + 25;
    ssd1306_draw_string(ssd, "Ohms", res_label_x + 25, label_y);

    // Linha 2: Valor Real
    ssd1306_draw_string(ssd, "RL:", res_label_x, value_y1);
    ssd1306_draw_string(ssd, str_measured_val, res_value_x, value_y1);

    // Linha 3: Valor Comercial (E24)
    ssd1306_draw_string(ssd, "CM:", res_label_x, value_y2);
    ssd1306_draw_string(ssd, str_standard_val, res_value_x, value_y2);

    ssd1306_send_data(ssd);
}