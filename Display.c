#include "Display.h"
#include "pico/stdlib.h"
#include "pico/mutex.h"
#include <string.h>
#include <stdio.h>

// Pinos do LCD
// Bit  7  6  5  4  3  2  1  0
// LCD  D7 D6 D5 D4 BL EN RW RS

#ifdef LCD_USE_I2C
    #include "hardware/i2c.h"

    #define LCD_BACKLIGHT 0b1000
    #define LCD_ENABLE 0b100

    static void lcd_hw_init(void) {
        // Liga o periférico de hardware I2C e configura o clock do 
        // barramento para 100kHz.
        i2c_init(LCD_I2C_PORT, 100*1000); 

        // No RP2350, cada pino tem um multiplexador interno.
        // gpio_set_function() desconecta o pino do modo GPIO comum (SIO)
        // e entrega o controle físico dele ao periférico de hardware I2C.
        gpio_set_function(LCD_PIN_SDA, GPIO_FUNC_I2C);
        gpio_set_function(LCD_PIN_SCL, GPIO_FUNC_I2C);

        // Ativa resistores de pull-up internos no SDA e  SCL
        gpio_pull_up(LCD_PIN_SDA);
        gpio_pull_up(LCD_PIN_SCL);
    }

    static void lcd_write_nibble(uint8_t nibble, bool is_data) {
        uint8_t data = ((nibble & 0x0f) << 4) | (is_data ? 0b1 : 0b0) | LCD_BACKLIGHT;
        uint8_t buf_high = data | LCD_ENABLE;
        uint8_t buf_low  = data & ~LCD_ENABLE;

        i2c_write_blocking(LCD_I2C_PORT, LCD_I2C_ADDR, &buf_high, 1, false);
        sleep_us(500);

        i2c_write_blocking(LCD_I2C_PORT, LCD_I2C_ADDR, &buf_low, 1, false);
        sleep_us(500);
    }

#else
    static void lcd_hw_init(void) {
        // 'uint' é um typedef do Pico SDK para 'unsigned int' (padrão em todas as funções de pino)
        const uint pins[] = {
            LCD_PIN_RS, LCD_PIN_E,
            LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7
        };

        for (int i = 0; i < 6; i++) {
            // Inicializa o pino no modo GPIO controlado por software (SIO),
            // zera o valor de saída e o configura inicialmente como entrada.
            gpio_init(pins[i]);

            // Define a direção do pino: GPIO_OUT (saída) ou GPIO_IN (entrada).
            gpio_set_dir(pins[i], GPIO_OUT);

            // Escreve nível lógico no pino: 0 = LOW (0 V), diferente de 0 = HIGH (3,3 V).
            gpio_put(pins[i], 0);
        }
    }

    static void lcd_write_nibble(uint8_t nibble, bool is_data) {
        // Escreve o nível lógico em cada pino individualmente (0 ou 1)
        gpio_put(LCD_PIN_RS, is_data);
        gpio_put(LCD_PIN_D4, (nibble >> 0) & 1);
        gpio_put(LCD_PIN_D5, (nibble >> 1) & 1);
        gpio_put(LCD_PIN_D6, (nibble >> 2) & 1);
        gpio_put(LCD_PIN_D7, (nibble >> 3) & 1);

        // Gera o pulso no pino Enable (E) para o HD44780 amostrar D4..D7
        gpio_put(LCD_PIN_E, 1);
        sleep_us(5);    // Pausa de 5 µs (tempo mínimo de pulso do HD44780 é ~450 ns)
        gpio_put(LCD_PIN_E, 0);
        sleep_us(50);   // Pausa de 50 µs (tempo para o LCD processar o comando interno)
    }
#endif

// ==========================================
// PROTOCOLO HD44780 (independente do Pico SDK, só usa sleep_ms)
// ==========================================

static void lcd_send_byte(uint8_t val, bool is_data) {
    lcd_write_nibble(val >> 4, is_data);   // Envia os 4 bits mais significativos
    lcd_write_nibble(val & 0x0F, is_data); // Envia os 4 bits menos significativos
}

void lcd_clear(void) {
    lcd_send_byte(0x01, false);
    // Pausa de 2 milissegundos (o comando Clear Display do HD44780 leva ~1,52 ms)
    sleep_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_send_byte(addr, false);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_send_byte((uint8_t)*str++, true);
    }
}

void display_init(void) {
    lcd_hw_init();
    sleep_ms(50); // Aguarda a estabilização da alimentação do LCD após ligar (> 40 ms)

    // Sequência obrigatória do datasheet do HD44780 para forçar entrada em 4 bits
    lcd_write_nibble(0x03, false);
    sleep_ms(5);
    lcd_write_nibble(0x03, false);
    sleep_us(150);
    lcd_write_nibble(0x03, false);
    lcd_write_nibble(0x02, false);

    // Comandos finais de configuração
    lcd_send_byte(0x28, false); // 4 bits, 2 linhas, fonte 5x8
    lcd_send_byte(0x0C, false); // Display ligado, cursor desligado
    lcd_send_byte(0x06, false); // Incrementa o cursor automaticamente para a direita
    lcd_clear();
}

static mutex_t lcd_mutex;
static char linha0[17] = "AiRM Flyer      ";
static char linha1[17] = "Iniciando...    ";
static volatile bool tela_dirty = true;

void display_init_sync(void) {
    mutex_init(&lcd_mutex);
}

void display_update(const char *l0, const char *l1) {
    mutex_enter_blocking(&lcd_mutex);
    snprintf(linha0, sizeof(linha0), "%-16s", l0);
    snprintf(linha1, sizeof(linha1), "%-16s", l1);
    tela_dirty = true;
    mutex_exit(&lcd_mutex);
}


void display_process(void) {
    if (!tela_dirty) {
        return; // Nada mudou, sai imediatamente
    }

    static char copia0[17];
    static char copia1[17];

    // Copia rápida dentro do mutex
    mutex_enter_blocking(&lcd_mutex);
    memcpy(copia0, linha0, sizeof(copia0));
    memcpy(copia1, linha1, sizeof(copia1));
    tela_dirty = false;
    mutex_exit(&lcd_mutex);

    // Escrita lenta no hardware fora do mutex
    lcd_set_cursor(0, 0);
    lcd_print(copia0);
    lcd_set_cursor(1, 0);
    lcd_print(copia1);
}