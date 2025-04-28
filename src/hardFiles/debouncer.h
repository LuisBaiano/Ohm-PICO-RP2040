  #ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/types.h" // Para uint32_t se time_us_32() for usado

/**
 * @brief Verifica se o tempo de debounce foi ultrapassado.
 */
bool check_debounce(uint32_t *last_interrupt_time, uint32_t debounce_time_us);

#endif // DEBOUNCER_H
