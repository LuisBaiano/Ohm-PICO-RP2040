
# Ohmímetro Inteligente com Identificação de Código de Cores E24 na BitDogLab

## Índice

1. [Objetivos](#objetivos)
2. [Descrição do Projeto](#descricao-do-projeto)
3. [Funcionalidades Implementadas](#funcionalidades-implementadas)
4. [Requisitos Técnicos Atendidos](#requisitos-tecnicos-atendidos)
5. [Como Executar](#como-executar)
6. [Estrutura do Código](#estrutura-do-codigo)
7. [Referências](#referencias)
8. [Demonstrativo em Vídeo](#demonstrativo-em-video)

## Objetivos

* Revisar e consolidar os conhecimentos sobre o microcontrolador RP2040 e a placa BitDogLab.
* Criar um Ohmímetro funcional utilizando o ADC para medição de resistência através de um divisor de tensão.
* Implementar a lógica para identificar o valor comercial padrão E24 (510Ω a 100kΩ) mais próximo ao valor medido.
* Decodificar o valor E24 padrão nas suas correspondentes 3 faixas de cores (1º dígito, 2º dígito, multiplicador).
* Integrar múltiplos periféricos da BitDogLab: ADC, GPIO (Botões com IRQ), I2C (Display OLED SSD1306) e PIO (Matriz de LEDs WS2812).
* Fornecer feedback visual claro no display OLED, mostrando nomes das cores, valor ADC, modo de operação (USB/BAT), resistência real medida e resistência comercial E24 identificada.
* Fornecer feedback visual complementar na matriz de LEDs, exibindo as cores das faixas decodificadas.
* Implementar chaveamento de modo de calibração ADC (USB/Bateria) via botão com tratamento de debounce e interrupção.
* Desenvolver o projeto em um ambiente VS Code configurado para o RP2040 com código modularizado e comentado.

## Descrição do Projeto

Este projeto implementa um "Ohmímetro Inteligente" na placa BitDogLab com o RP2040. O sistema mede a resistência de um componente externo conectado a um divisor de tensão (usando um resistor fixo interno de 10kΩ).

Após a leitura da tensão via ADC, o sistema calcula a resistência real (`r_medido`). Em seguida, compara este valor com a série padrão E24 para encontrar o valor comercial mais próximo (`r_padrao`) dentro da faixa de 510Ω a 100kΩ. As três primeiras faixas de cores correspondentes a este valor padrão são então determinadas.

O display OLED SSD1306 (128x64, via I2C) é o principal meio de feedback, exibindo:

* Nomes das 3 faixas de cores (ou "-------" se inválido).
* O valor bruto lido do ADC.
* O modo de calibração ADC ativo ("USB" ou "BAT").
* O valor da resistência REAL medida (completo, em Ohms).
* O valor COMERCIAL E24 identificado (em Ohms, ou status como "Aberto"/"Curto"/"N/D E24").

A matriz de LEDs 5x5 WS2812 (controlada via PIO) oferece um feedback visual adicional, acendendo as colunas 1, 3 e 5 com as cores da 1ª faixa, 2ª faixa e multiplicador, respectivamente, com brilho ajustável via software.

O Botão A permite ao usuário alternar entre dois modos de calibração para o cálculo da resistência, correspondendo à alimentação via USB (máx. ADC 4045) ou Bateria (máx. ADC 3620), com detecção via interrupção e debounce. O Botão B é configurado para ativar o modo BOOTSEL para fácil reprogramação. Mensagens de log e status são enviadas via `printf` (visível em terminal serial USB).

## Funcionalidades Implementadas

```
✅ Leitura analógica via ADC (GPIO 28) do divisor de tensão.
✅ Cálculo de média de múltiplas amostras ADC para redução de ruído.
✅ Cálculo da resistência real (`r_medido`) baseado na fórmula do divisor e no modo ADC selecionado (USB/BAT).
✅ Identificação do valor comercial E24 mais próximo (`r_padrao`) na faixa 510Ω-100kΩ.
✅ Decodificação das 3 faixas de cores (dígitos e multiplicador) a partir do valor E24 padrão.
✅ Exibição no display OLED (I2C): Nomes das cores, Valor ADC, Modo (USB/BAT), Resistência REAL (Ohm), Resistência COMERCIAL (Ohm) ou status (Aberto/Curto/N/D E24).
✅ Exibição na Matriz de LEDs (PIO): Cores das 3 faixas nas colunas físicas 1, 3 e 5 (com mapeamento físico-lógico corrigido e brilho ajustável).
✅ Leitura do Botão A (GPIO 5) via Interrupção (IRQ) com tratamento de Debounce para alternar `current_adc_max_value`.
✅ Função BOOTSEL no Botão B (GPIO 6) via Interrupção (IRQ) com Debounce.
✅ Mensagens de log (inicialização, mudança de modo ADC) enviadas via `printf` (stdio/USB).
✅ Código estruturado em múltiplos módulos (`main.c`, `ohmmeter`, `display`, `led_matrix`, `buttons`, `debouncer`) para organização e clareza.
✅ Código comentado explicando as principais seções e lógicas.
```

## Requisitos Técnicos Atendidos

*(Baseado nos requisitos do enunciado original do trabalho)*

1. **Realizar a leitura do resistor:** Sim, implementado via leitura ADC (GPIO 28) do divisor de tensão.
2. **Calcular o valor da resistência:** Sim, implementado usando a fórmula do divisor e o `avg_adc`, considerando o modo USB/BAT.
3. **Identificar o valor comercial padrão de 5% mais próximo (E24):** Sim, função `find_closest_e24` busca o valor na lista E24 dentro da faixa 510Ω-100kΩ.
4. **Determinar as cores das faixas:** Sim, função `get_resistor_colors` decodifica o valor E24 nas 3 cores principais.
5. **Exibir no display:**
   * **Valor numérico:** Sim, exibe `R_medido` (REAL) e `R_padrao` (COM).
   * **Três primeiras faixas de cores:** Sim, exibe os nomes por extenso ("Faixa 1:", "Faixa 2:", "Multi:").
   * **Desenho gráfico (Opcional):** Um desenho genérico foi incluído na tela de inicialização.
   * **Uso da Matriz de LEDs (Opcional):** Sim, utilizada para representar visualmente as 3 cores das faixas nas colunas 1, 3 e 5.
6. **Código bem estruturado e comentado:** Sim, o projeto foi organizado em módulos com arquivos `.c`/`.h` e comentários foram adicionados.

## Como Executar

### Requisitos de Hardware

* Placa de desenvolvimento **BitDogLab** (com RP2040).
* Cabo Micro-USB para conexão e alimentação/programação.
* **Protoboard.**
* **Fios Jumper.**
* **Resistor de 10kΩ** (para `R_DIVISOR` - meça o valor real para melhor precisão!).
* **Resistores diversos** na faixa de 510Ω a 100kΩ para teste.

### Requisitos de Software

* **VS Code** com a extensão Pico-W-Go ou configuração manual do toolchain ARM e Pico SDK.
* **Pico SDK** versão compatível instalada e configurada (ex: 1.5.1 ou mais recente).
* **Git** (opcional, para clonar).
* Um **Terminal Serial** (ex: Monitor Serial do VS Code, Putty, Minicom, Tera Term) para visualizar os logs (`printf`).

### Passos

1. **Clonar ou Baixar o Repositório:**
   ```bash
   git clone https://github.com/LuisBaiano/Ohm-PICO-RP2040.git
   cd Ohm-PICO-RP2040
   ```
2. **Configurar o Projeto:**
   * Abra a pasta do projeto no VS Code.
   * **IMPORTANTE:** Verifique e ajuste o valor de `#define R_DIVISOR` no arquivo `src/config.h` com o valor **real medido** do seu resistor de 10kΩ.
   * **IMPORTANTE:** Verifique e ajuste os valores `#define ADC_MAX_USB` (4045) e `#define ADC_MAX_BATTERY` (3620) em `src/config.h` com os valores máximos **reais observados** no seu ADC em circuito aberto para cada modo de alimentação.
   * Verifique se o arquivo `CMakeLists.txt` está correto e se o SDK do Pico está acessível pelo seu ambiente.
3. **Compilar (Build):**
   * Via VS Code: Use a função de build integrada (ex: F7, Ctrl+Shift+B, ou botão na barra de status do CMake Tools).
   * Via Linha de Comando (dentro da pasta do projeto):
     ```bash
     mkdir build
     cd build
     cmake ..
     make
     ```
4. **Carregar o Firmware:**
   * Desconecte a BitDogLab do USB.
   * Pressione e segure o botão **BOOTSEL** (associado ao Botão B/GPIO 6 no nosso código).
   * Conecte a BitDogLab ao computador via cabo Micro-USB enquanto mantém o botão BOOTSEL pressionado.
   * Solte o botão BOOTSEL. A placa deve aparecer como um drive USB chamado `RPI-RP2`.
   * Arraste e solte o arquivo `.uf2` gerado na compilação (estará dentro da pasta `build`) para dentro do drive `RPI-RP2`.
   * A placa será programada e reiniciará automaticamente executando o novo firmware.
5. **Visualizar Logs:** Abra seu terminal serial, selecione a porta serial correta para a Pico e configure a taxa para **115200 baud**. Você verá as mensagens de inicialização.
6. **Testar:**
   * Monte o circuito do divisor de tensão na protoboard, conectando 3V3, GND e GP28 da BitDogLab conforme descrito anteriormente.
   * Insira diferentes resistores (na faixa 510Ω-100kΩ) no local de R_X.
   * Observe as leituras no Display OLED e as cores na Matriz de LEDs.
   * Pressione o Botão A (GPIO 5) para alternar entre os modos "USB" e "BAT" e veja a mudança no display e nos logs.
   * (Se precisar reprogramar) Pressione o Botão B (GPIO 6) ao conectar para entrar em modo BOOTSEL.

## Estrutura do Código

```
📂 Ohm-PICO-RP2040/
├── src/                     # Pasta principal do código fonte
│   ├── hardFiles/           # Módulos específicos do hardware/aplicação
│   │   ├── ohmmeter.c
│   │   ├── ohmmeter.h
│   │   ├── display.c
│   │   ├── display.h
│   │   ├── led_matrix.c
│   │   ├── led_matrix.h
│   │   ├── buttons.c
│   │   ├── buttons.h
│   │   └── debouncer.c
│   │   └── debouncer.h
│   ├── lib/                 # Bibliotecas externas ou adaptadas
│   │   ├── ssd1306.c      # Implementação do driver SSD1306
│   │   ├── ssd1306.h      # Header do driver SSD1306
│   │   └── font.c         # Dados da fonte (ou pode estar embutido em ssd1306.c)
│   │   └── font.h         # Header da fonte (se separada)
│   ├── config.h           # Definições globais de pinos e constantes
│   └── main.c             # Ponto de entrada, loop principal, inicialização
│
├── pio/led_matrix.pio           # Arquivo fonte do programa PIO para WS2812
├── CMakeLists.txt           # Arquivo de configuração do build CMake
├── pico_sdk_import.cmake    # Script padrão para importar o SDK do Pico
└── (outros arquivos, ex: .gitignore)
```

## Referências

* [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
* [Raspberry Pi Pico C/C++ SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
* [Resistor Color Code Calculator](https://www.calculator.net/resistor-color-code-calculator.html) (Útil para verificar cores)
* [E series of preferred numbers (E3, E6, E12, E24, E48, E96)](https://en.wikipedia.org/wiki/E_series_of_preferred_numbers) (Informação sobre séries E)

## Demonstrativo em Vídeo

**[📹 Link para o Vídeo de Demonstração]**(Substitua este texto pelo link real do seu vídeo no YouTube ou Google Drive)
