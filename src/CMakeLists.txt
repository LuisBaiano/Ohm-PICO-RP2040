
set(PICO_BOARD pico_w CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)


project(Projeto_Ohmmimetro C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Inicializa o SDK
pico_sdk_init()

# === Adiciona o Executável e Fontes PRIMEIRO ===
add_executable(main
    main.c
    hardFiles/ohmmeter.c
    hardFiles/display.c
    hardFiles/led_matrix.c
    hardFiles/buttons.c
    hardFiles/debouncer.c
    lib/ssd1306.c
)

# === Gera o pio.h para controlar a matriz de leds ===
pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/pio/led_matrix.pio)

# === Adiciona diretórios de include ===
target_include_directories(main PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib      
    ${CMAKE_CURRENT_LIST_DIR}/hardFiles 
    ${CMAKE_BINARY_DIR}                   
)

# === Linka Bibliotecas do SDK ===
target_link_libraries(main PUBLIC
    pico_stdlib
    hardware_adc
    hardware_i2c
    hardware_pio
    hardware_gpio
    hardware_irq
    hardware_clocks
    pico_bootrom
    m         
)

pico_add_extra_outputs(main)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)
