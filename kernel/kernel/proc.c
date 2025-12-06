#include <stddef.h>
#include "../include/proc.h"
#include "../include/lottery.h"
#include "../include/context.h"

#define MAX_PROCESSES 10

static struct pcb processes[MAX_PROCESSES];
static struct pcb* current = NULL;

struct pcb* get_current_pcb(void) { return current; }
void set_current_pcb(struct pcb* p) { current = p; }

void init_process_system() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = i;
        processes[i].state = PROC_UNUSED;
        processes[i].ctx = NULL;
        processes[i].tickets = 0;
        processes[i].next = NULL;
    }
}

// struct pcb* create_process(void (*entry)(), int pid) {
//     struct pcb* p = &processes[pid];
// 
//     static struct context contexts[MAX_PROCESSES];
//     p->ctx = &contexts[pid];
// 
//     // Allocate stack
//     static uint32_t stacks[MAX_PROCESSES][1024];
//     uint32_t *stack = stacks[pid] + 1024;
// 
//     *(--stack) = (uint32_t)entry; // fake return address
// 
//     p->ctx->esp = (uint32_t)stack;
// 
//     p->state = PROC_RUNNABLE;
//     p->tickets = 10;
// 
//     lottery_add_process(p);
// 
//     return p;
// }

struct pcb* create_process(void (*entry)(), int pid) {
    struct pcb* p = &processes[pid];

    static struct context contexts[MAX_PROCESSES];
    p->ctx = &contexts[pid];

    static uint32_t stacks[MAX_PROCESSES][1024];
    uint32_t *stack = stacks[pid] + 1024;

    // push registers expected by popa
    *(--stack) = 0;  // EDI
    *(--stack) = 0;  // ESI
    *(--stack) = 0;  // EBP
    *(--stack) = 0;  // dummy ESP
    *(--stack) = 0;  // EBX
    *(--stack) = 0;  // EDX
    *(--stack) = 0;  // ECX
    *(--stack) = 0;  // EAX

    // push return address for ret → entry function
    *(--stack) = (uint32_t)entry;

    p->ctx->esp = (uint32_t)stack;
    p->state = PROC_RUNNABLE;
    p->tickets = 10;

    lottery_add_process(p);
    return p;
}
void proc1(void) {
    volatile int x = 0;
    while (1) {
        x++;
    }
}

void proc2(void) {
    volatile int y = 0;
    while (1) {
        y += 2;
    }
}

void idle_task(void) {
    while (1) {
        __asm__ volatile("hlt");
    }
}

