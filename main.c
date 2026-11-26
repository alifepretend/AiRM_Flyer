#include <stdio.h>
#include "pico/stdlib.h"
#include "Core1.h"
#include "Display.h"

//#define USE_CORE1

int main() {
    stdio_init_all();

    // Cria o mutex para não dar problema de sincronização entre os cores
    display_init_sync();
    core1_start();

    while (true) {
        display_update("MODO MANUAL", "T: 180C  05:00");
        sleep_ms(500);

    #ifndef USE_CORE1
        display_process(); // No Wokwi
    #endif
    }
}
