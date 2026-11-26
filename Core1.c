#include "Core1.h"
#include "Display.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

static void core1_entry(void) {
    // Inicializa os periféricos que pertencem ao Core 1
    display_init();

    while (true) {
        display_process();
        // Futuramente: buzzer_process(); leds_process();

        sleep_ms(50); // Taxa máxima de atualização de 20 Hz
    }
}

void core1_start(void) {
    #ifdef USE_CORE1
    multicore_launch_core1(core1_entry);
#else
    // Para simular no wokwi
    display_init(); 
#endif
}