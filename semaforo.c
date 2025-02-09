#include "pico/stdio.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// pinos dos LEDs 
#define LED_RED 11
#define LED_YELLOW 12
#define LED_GREEN 13

// tempo de troca do semáforo 
#define TIMER_PERIOD_MS 3000

volatile int state = 0;

// função de callback do temporizador
bool traffic_light_callback(struct repeating_timer *t) {
    gpio_put(LED_RED, 0);
    gpio_put(LED_YELLOW, 0);
    gpio_put(LED_GREEN, 0);

    // Liga o LED correspondente ao estado atual
    if (state == 0) {
        gpio_put(LED_RED, true);    
        printf("Semáforo: VERMELHO\n");
    } else if (state == 1) {
        gpio_put(LED_YELLOW, true); 
        printf("Semáforo: AMARELO\n");
    } else {
        gpio_put(LED_GREEN, true);  
        printf("Semáforo: VERDE\n");
    }

    // alterna para o próximo estado
    state = (state + 1) % 3;

    return true; // mantém o temporizador repetindo
}

int main() {
    stdio_init_all(); 

   
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_YELLOW);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    // inicializa o temporizador periódico para alternar o semáforo
    struct repeating_timer timer;
    add_repeating_timer_ms(TIMER_PERIOD_MS, traffic_light_callback, NULL, &timer);

    // loop principal para imprimir mensagens a cada 1 segundo
    while (true) {
        
        printf("repeticao \n");
        sleep_ms(1000); // Espera 1 segundo antes de imprimir novamente
    }

    return 0;
}
