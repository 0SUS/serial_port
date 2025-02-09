# Projeto: Controle de Display SSD1306 e Matriz de LEDs com BitDogLab

## Descrição
Este projeto utiliza a placa **BitDogLab** para exibir caracteres no **display SSD1306** via **UART**, controlar LEDs comuns e uma matriz de LEDs WS2812. O projeto também implementa **interrupções** para manipulação de botões e **debounce** para evitar leituras incorretas.

## Funcionalidades
1. **Entrada de caracteres via PC**
   - O caractere digitado no **Serial Monitor** do VS Code é exibido no **display SSD1306**.
   - Se for um número entre **0 e 9**, ele também será representado na **matriz de LEDs WS2812**.

2. **Interação com o Botão A**
   - Alterna o estado do **LED Verde** (GPIO 11).
   - Exibe a mensagem no **display SSD1306**.
   - Envia uma mensagem ao **Serial Monitor**.

3. **Interação com o Botão B**
   - Alterna o estado do **LED Azul** (GPIO 12).
   - Exibe a mensagem no **display SSD1306**.
   - Envia uma mensagem ao **Serial Monitor**.

## Requisitos do Projeto
1. **Uso de interrupções** para manipular os botões.
2. **Implementação de debounce** para evitar leituras erradas.
3. **Controle de LEDs WS2812 e LEDs comuns**.
4. **Utilização do Display SSD1306** via I2C com suporte a caracteres maiúsculos e minúsculos.
5. **Envio e recebimento de dados pela UART**.

## Componentes Utilizados
- **Placa BitDogLab**
- **Display OLED SSD1306** (I2C - GPIO 14 e 15)
- **Matriz de LEDs WS2812** (GPIO 7)
- **LED RGB** (GPIO 11 e 12)
- **Botões** (GPIO 5 e 6)
- **UART** para comunicação com o PC (GPIO 0 e 1)

## Configuração do Hardware
### Ligação dos Componentes:
| Componente       | GPIO |
|-----------------|------|
| Display SSD1306 (SDA) | 14   |
| Display SSD1306 (SCL) | 15   |
| Matriz WS2812   | 7    |
| LED Verde       | 11   |
| LED Azul        | 12   |
| Botão A        | 5    |
| Botão B        | 6    |
| UART TX         | 0    |
| UART RX         | 1    |

## Como Executar o Projeto
1. **Compilar e carregar o código** na placa BitDogLab.
2. **Abrir o Serial Monitor** do VS Code para visualizar as mensagens e enviar caracteres.
3. **Pressionar os botões** para alternar os LEDs.
4. **Enviar números entre 0 e 9** pelo Serial Monitor para exibi-los na matriz de LEDs.

## Autor
Desenvolvido por João Pedro Ferreira de Jesus como parte de um estudo sobre comunicação serial e controle de hardware com a placa **BitDogLab**.

