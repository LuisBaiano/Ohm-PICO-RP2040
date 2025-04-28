#include "debouncer.h"
#include "pico/time.h" // Adicione se time_us_32 não estiver em pico/types

/**
 * @brief Verifica se o tempo de debounce foi ultrapassado.
 */
bool check_debounce(uint32_t *last_interrupt_time, uint32_t debounce_time_us) {
    uint32_t current_time = time_us_32(); // Usa a função do SDK do RPi Pico
    if ((current_time - *last_interrupt_time) > debounce_time_us) {
        *last_interrupt_time = current_time;
        return true;
    }
    return false;
}
