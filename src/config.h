#ifndef CONFIG_H
#define CONFIG_H

#include "hardware/i2c.h"

// === Pinos I2C (OLED) ===
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define OLED_I2C_ADDR 0x3C

// === Pino ADC (Ohmímetro) ===
#define ADC_GPIO_PIN 28
#define ADC_CHANNEL 2

// === Constantes Ohmímetro ===
#define R_DIVISOR 10000.0f
#define ADC_REF_VOLTAGE 3.3f

// === Valores Máximos ADC ===
#define ADC_MAX_USB     4045
#define ADC_MAX_BATTERY 3630

// === Pinos Botões ===
#define BUTTON_A_PIN 5 // Usado para trocar modo ADC
//#define BUTTON_B_PIN 6 // Usado para BOOTSEL IRQ

// === Pino e Configurações Matriz de LEDs ===
#define MATRIX_LED_PIN 7
#define MATRIX_PIO_INSTANCE pio0
#define MATRIX_PIO_SM 0

// === Constantes de Temporização ===
#define ADC_SAMPLES 200
#define BUTTON_DEBOUNCE_US 200000


#endif // CONFIG_H