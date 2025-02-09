#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"
#include "ssd1306.h"  // Biblioteca para o display
#include "ws2812.h"    // Biblioteca para a matriz de LEDs WS2812
#include "font.h"

// Definições dos pinos
#define LED_R 11
#define LED_G 12
#define LED_B 13
#define BUTTON_A 5
#define BUTTON_B 6
#define WS2812_PIN 7
#define I2C_SDA 14
#define I2C_SCL 15
#define UART_TX 0
#define UART_RX 1

// Variáveis globais
volatile bool led_green_state = false;
volatile bool led_blue_state = false;

// Inicialização do Display SSD1306
ssd1306_t disp;

// Inicialização da Matriz WS2812
ws2812_t matrix;

// Função para debounce
bool debounce(uint gpio) {
    static absolute_time_t last_press_time = {0};
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_press_time, now) > 200000) { // 200ms debounce
        last_press_time = now;
        return true;
    }
    return false;
}

// Callback do botão A
void button_a_callback(uint gpio, uint32_t events) {
    if (debounce(gpio)) {
        led_green_state = !led_green_state;
        gpio_put(LED_G, led_green_state);
        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 0, 0, led_green_state ? "LED Verde ON" : "LED Verde OFF");
        ssd1306_show(&disp);
        printf("Botão A pressionado. LED Verde %s\n", led_green_state ? "Ligado" : "Desligado");
    }
}

// Callback do botão B
void button_b_callback(uint gpio, uint32_t events) {
    if (debounce(gpio)) {
        led_blue_state = !led_blue_state;
        gpio_put(LED_B, led_blue_state);
        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 0, 0, led_blue_state ? "LED Azul ON" : "LED Azul OFF");
        ssd1306_show(&disp);
        printf("Botão B pressionado. LED Azul %s\n", led_blue_state ? "Ligado" : "Desligado");
    }
}

// Função para exibir número na matriz WS2812
void display_number_ws2812(int num) {
    ws2812_clear(&matrix);
    ws2812_draw_number(&matrix, num);
    ws2812_show(&matrix);
}

// Função para processar entrada UART
void process_uart_input() {
    if (uart_is_readable(uart0)) {
        char received = uart_getc(uart0);
        printf("Recebido: %c\n", received);

        ssd1306_clear(&disp);
        char str[2] = {received, '\0'};
        ssd1306_draw_string(&disp, 0, 0, str);
        ssd1306_show(&disp);

        if (received >= '0' && received <= '9') {
            display_number_ws2812(received - '0');
        }
    }
}

int main() {
    stdio_init_all();

    // Configuração dos LEDs
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);

    // Configuração dos botões com interrupção
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_a_callback);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_b_callback);

    // Inicialização do display SSD1306
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&disp, 128, 64);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 0, 0, "Sistema Iniciado");
    ssd1306_show(&disp);

    // Inicialização da matriz WS2812
    ws2812_init(&matrix, WS2812_PIN, 5, 5);

    // Inicialização da UART
    uart_init(uart0, 115200);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);

    while (1) {
        process_uart_input();
        sleep_ms(100);
    }

    return 0;
}
