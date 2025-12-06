#include <stdint.h>
#include "../include/lottery.h"
#include "../include/random.h" 
#include "../include/proc.h"
#include "../include/interrupts.h"
#include "../include/timer.h"

static inline void clear_vga(void) {
    uint16_t *vga = (uint16_t*)0xB8000;
    uint16_t blank = (0x07 << 8) | ' ';  // 0x0720

    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = blank;
    }
}

// void kernel_main(void){
// 	clear_vga();
//    
//     rng_init(); 
//     lottery_init();
//     init_process_system();
// 
//     idt_init();
//     init_timer();
//     enable_interrupts();
// 
//     // Create one or two test processes
//     create_process(process1_entry, 1);
//     create_process(process2_entry, 2);
// 
//     // Idle loop: scheduler switches tasks on timer interrupts
// 	for(;;) __asm__ volatile("hlt");
// }

void kernel_main(void){
    clear_vga();

    rng_init();
    lottery_init();
    init_process_system();

    idt_init();
    extern void pic_remap(void);
    extern void pic_unmask_timer(void);

    pic_remap();
    pic_unmask_timer();
    init_timer();
    enable_interrupts();

    // -------------------------
    // Create idle task (PID 0)
    // -------------------------
    extern void idle_task();
    create_process(idle_task, 0);

    // -------------------------
    // TEST PROCESSES GO HERE
    // -------------------------
    extern void proc1();
    extern void proc2();
    create_process(proc1, 1);
    create_process(proc2, 2);

    // -------------------------
    // Now the scheduler runs on every timer tick
    // -------------------------
    while (1) {
        __asm__ volatile("hlt");
    }
}

