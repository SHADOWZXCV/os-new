#include "init.h"
#include "IO/keyboard.h"
#include "memory/memory.h"
#include "interrupts/time.h"
#include "IO/io.h"

void initOs() {
    enable_clock_timer_chip();
    set_keyboard_leds();
    mmanager_init_memory();
    clrscr();
}
