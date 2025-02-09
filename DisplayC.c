//Inclui as bibliotecas necessárias
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Arquivo .pio
#include "DisplayC.pio.h"

//Definições de pinos e configurações
#define I2C_PORT i2c1
#define I2C_SDA 14  // Pino de dados do I2C
#define I2C_SCL 15  // Pino de clock do I2C
#define DISPLAY_ADDR 0x3C  // Endereço I2C do display SSD1306
#define MATRIX_LED_PIN 7  // Pino da matriz de LEDs WS2812
#define LED_G 11  // LED Verde
#define LED_B 12  // LED Azul
#define BUTTON_A 5  // Botão A
#define BUTTON_B 6  // Botão B
#define UART_PORT uart0  // Porta UART utilizada
#define UART_TX 0  // Pino de transmissão UART
#define UART_RX 1  // Pino de recepção UART
// Número de LEDs
#define NUM_PIXELS 25
// Pino de saída matriz
#define OUT_PIN 7

//Estrutura do display SSD1306
ssd1306_t ssd;
volatile bool led_g_state = false; // Estado do LED Verde
volatile bool led_b_state = false; // Estado do LED Azul
volatile char received_char = ' '; // Caractere recebido via UART

// Função para tratar o debounce dos botões
void debounce_handler(uint gpio, uint32_t events) {
    static absolute_time_t last_time_a = 0;
    static absolute_time_t last_time_b = 0;
    absolute_time_t now = get_absolute_time();

    // Tratamento do botão A
    if (gpio == BUTTON_A && absolute_time_diff_us(last_time_a, now) > 200000) {
        last_time_a = now;
        led_g_state = !led_g_state; // Alterna o estado do LED Verde
        gpio_put(LED_G, led_g_state); // Atualiza o LED Verde
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, led_g_state ? "LED Verde ON" : "LED Verde OFF", 0, 0);
        ssd1306_send_data(&ssd);
        uart_puts(UART_PORT, led_g_state ? "LED Verde ligado\n" : "LED Verde desligado\n");
        printf("%s", led_g_state ? "LED Verde ligado\n" : "LED Verde desligado\n");
    }
    
    // Tratamento do botão B
    if (gpio == BUTTON_B && absolute_time_diff_us(last_time_b, now) > 200000) {
        last_time_b = now;
        led_b_state = !led_b_state; // Alterna o estado do LED Azul
        gpio_put(LED_B, led_b_state); // Atualiza o LED Azul
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, led_b_state ? "LED Azul ON" : "LED Azul OFF", 0, 0);
        ssd1306_send_data(&ssd);
        uart_puts(UART_PORT, led_b_state ? "LED Azul ligado\n" : "LED Azul desligado\n");
        printf("%s", led_b_state ? "LED Azul ligado\n" : "LED Azul desligado\n");
        
        
    }
}



//Numeros na matriz de leds 5x5

// Estrutura para armazenar dados do numero
typedef struct {
  double frames[10][NUM_PIXELS];
  int num_frames;
  double r, g, b;
  int fps; 
} Animacao;

// Função para criar cor RGB
uint32_t matrix_rgb(double b, double r, double g) {
  unsigned char R = r * 255;
  unsigned char G = g * 255;
  unsigned char B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

// Inicializa o PIO para a matriz de LEDs
void init_matriz_led(PIO pio, uint *offset, uint *sm) {
  *offset = pio_add_program(pio, &DisplayC_program);
  if (*offset < 0) {
      
      return;
  }

  *sm = pio_claim_unused_sm(pio, true);
  if (*sm < 0) {
      
      return;
  }

  DisplayC_program_init(pio, *sm, *offset, OUT_PIN);
}

void executar_animacao(PIO pio, uint sm, Animacao *anim) {
  
  for (int frame = 0; frame < anim->num_frames; frame++) {
      for (int i = 0; i < NUM_PIXELS; i++) {
          double intensidade = anim->frames[frame][i];
          uint32_t valor_led = matrix_rgb(anim->b * intensidade, anim->r * intensidade, anim->g * intensidade);
          pio_sm_put_blocking(pio, sm, valor_led);
      }

      
  }
}

// Configuração de animações
Animacao animacao_noone = {
  .frames = {
      {
      0.0, 0.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 0.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 0.0,
  .g = 0.0,
  .b = 0.0,
  .fps = 1
};

Animacao animacao_0 = {
  .frames = {
      {
      1.0, 1.0, 1.0, 1.0, 1.0, 
      1.0, 0.0, 0.0, 0.0, 1.0, 
      1.0, 0.0, 0.0, 0.0, 1.0, 
      1.0, 0.0, 0.0, 0.0, 1.0, 
      1.0, 1.0, 1.0, 1.0, 1.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.0,
  .b = 0.0,
  .fps = 1
};

Animacao animacao_1 = {
  .frames = {
      {
      1.0, 0.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 0.0, 1.0, 
      1.0, 0.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 0.0, 1.0, 
      1.0, 0.0, 0.0, 0.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.005,
  .b = 0.05,
  .fps = 1
};

Animacao animacao_2 = {
  .frames = {
      {
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 1.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.01,
  .b = 0.1,
  .fps = 1
};

Animacao animacao_3 = {
  .frames = {
      {
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.02,
  .b = 0.15,
  .fps = 1
};

Animacao animacao_4 = {
  .frames = {
      {
      0.0, 1.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 1.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.05,
  .b = 0.2,
  .fps = 1
};

Animacao animacao_5 = {
  .frames = {
      {
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 0.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.09,
  .b = 0.25,
  .fps = 1
};

Animacao animacao_6 = {
  .frames = {
      {
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 0.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.1,
  .b = 0.3,
  .fps = 1
};

Animacao animacao_7 = {
  .frames = {
      {
      0.0, 1.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.14,
  .b = 0.35,
  .fps = 1
};

Animacao animacao_8 = {
  .frames = {
      {
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.16,
  .b = 0.4,
  .fps = 1
};

Animacao animacao_9 = {
  .frames = {
      {
      0.0, 1.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      0.0, 1.0, 0.0, 1.0, 0.0, 
      0.0, 1.0, 1.0, 1.0, 0.0, 
      }
      
  },
  .num_frames = 1,
  .r = 1.0,
  .g = 0.2,
  .b = 0.5,
  .fps = 1
};

//relaciona o numero a o que e é exibido na matriz de leds
void display_number(int8_t num, PIO pio, uint sm){
  switch(num){
      case 0: executar_animacao(pio, sm, &animacao_0);break;
      case 1: executar_animacao(pio, sm, &animacao_1);break;
      case 2: executar_animacao(pio, sm, &animacao_2);break;
      case 3: executar_animacao(pio, sm, &animacao_3);break;
      case 4: executar_animacao(pio, sm, &animacao_4);break;
      case 5: executar_animacao(pio, sm, &animacao_5);break;
      case 6: executar_animacao(pio, sm, &animacao_6);break;
      case 7: executar_animacao(pio, sm, &animacao_7);break;
      case 8: executar_animacao(pio, sm, &animacao_8);break;
      case 9: executar_animacao(pio, sm, &animacao_9);break;
      case 10: executar_animacao(pio, sm, &animacao_noone);break;
  }


}

// Função chamada na interrupção da UART para receber caracteres
void on_uart_rx() {
  while (uart_is_readable(UART_PORT)) {
      received_char = uart_getc(UART_PORT); // Lê o caractere recebido
      ssd1306_fill(&ssd, false);
      ssd1306_draw_char(&ssd, received_char, 0, 0); // Exibe o caractere no display
      ssd1306_send_data(&ssd);

      // Se o caractere recebido for um número entre '0' e '9'
      if (received_char >= '0' && received_char <= '9') {
        int numero = received_char - '0'; // Converte o caractere para número inteiro
        display_number(numero, pio0, 0);  // Exibe o número na matriz de LEDs
    }
  }
}

//main

int main() {
    
    PIO pio = pio0;
    uint offset, sm;

    stdio_init_all(); // Inicializa a comunicação padrão
    init_matriz_led(pio, &offset, &sm);

    // Inicializa a comunicação I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display SSD1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração dos LEDs
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);

    // Configuração dos botões com interrupção e debounce
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &debounce_handler);
    
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &debounce_handler);

    // Configuração da UART
    uart_init(UART_PORT, 115200);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_PORT, false, false);
    uart_set_format(UART_PORT, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_PORT, false);
    uart_set_irq_enables(UART_PORT, true, false);
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);

    
    

    // Loop principal
    while (true) {
      
      if(stdio_usb_connected()){
        char c;
        if(scanf("%c",&c) == 1){
          ssd1306_draw_char(&ssd,c,0,0);
          ssd1306_send_data(&ssd);

          if(c >= '0' && c <= '9'){
            int8_t numero = c-'0';

            display_number(numero, pio, sm);
          }else{display_number(10, pio, sm);}
        }
      }
        

        
    }
}
