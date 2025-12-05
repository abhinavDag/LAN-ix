#include <stdint.h>
#include "../include/lottery.h"

static inline void clear_vga(void) {
    uint16_t *vga = (uint16_t*)0xB8000;
    uint16_t blank = (0x07 << 8) | ' ';  // 0x0720

    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = blank;
    }
}

void kernel_main(void){
	clear_vga();
    lottery_init();
    volatile uint16_t* vga = (uint16_t*) 0xB8000;
	const char* message = "Hello from LAN-ix!";

	for(int i=0; message[i]; ++i){
		vga[i]  = (uint16_t)message[i] | (0x0700);
	}
	for(;;) __asm__ volatile("hlt");
}
