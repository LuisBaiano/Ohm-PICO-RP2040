#include "buttons.h"
#include "config.h"
#include "debouncer.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/irq.h"

// flag pro botão As
volatile bool g_button_a_pressed_flag = false;

// === Timers para Debounce ==
static uint32_t last_a_press_time = 0;
static uint32_t last_b_press_time = 0;

// Função de callback para interrupção GPIO (Trata Botão A e B)
void button_irq_callback(uint gpio, uint32_t events) {
    // Trata Botão A
    if (gpio == BUTTON_A_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        if (check_debounce(&last_a_press_time, BUTTON_DEBOUNCE_US)) {
            g_button_a_pressed_flag = true;
        }
    }
    // Trata Botão B (BOOTSEL)
    //else if (gpio == BUTTON_B_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
    //    if (check_debounce(&last_b_press_time, BUTTON_DEBOUNCE_US)) {
    //        reset_usb_boot(0, 0);
    //    }
    //}
}

// Inicializa os pinos GPIO dos botões E configura IRQ para Botão A
void init_buttons_gpio(void) {
    // Botão A
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_irq_callback);
}

// Configura a interrupção específica para o BOOTSEL no botão B
void init_bootsel_button(void) {
    // Garante que o GPIO está inicializado
    //gpio_init(BUTTON_B_PIN);
    //gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    //gpio_pull_up(BUTTON_B_PIN);
    //gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &button_irq_callback);
}