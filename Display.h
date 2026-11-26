#pragma once

#include <stdint.h>
#include <stdbool.h>

// ==========================================
// Para escolher entre I2C ou barramento, comentar ou descomentar
// a linha:
//     LCD_USE_I2C=1
// no arquivo CMakeLists.txt
// ==========================================

#ifdef LCD_USE_I2C
    #define LCD_I2C_PORT i2c0
    #define LCD_PIN_SDA  4
    #define LCD_PIN_SCL  5
    #define LCD_I2C_ADDR 0x27
#else
    #define LCD_PIN_RS   0
    #define LCD_PIN_E    1
    #define LCD_PIN_D4   2
    #define LCD_PIN_D5   3
    #define LCD_PIN_D6   4
    #define LCD_PIN_D7   5
#endif

// Chamada pelo Core 0 (inicializa o mutex antes de lançar o Core 1)
void display_init_sync(void);

// Chamada pelo Core 0 sempre que a FSM quiser mudar o texto (thread-safe)
void display_update(const char *l0, const char *l1);

// Chamadas exclusivamente pelo Core 1
void display_init(void);   // Inicializa I2C/GPIO e o HD44780
void display_process(void);   // Se houver dado novo, envia para o LCD