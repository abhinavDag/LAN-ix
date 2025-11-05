#include <stdint.h>

void kernel_main(void){
	volatile uint16_t* vga = (uint16_t*) 0xB8000;
	const char* message = "Hello from LAN-ix!";

	for(int i=0; message[i]; ++i){
		vga[i]  = (uint16_t)message[i] | (0x0700);
	}
	for(;;) __asm__ volatile("hlt");
}
