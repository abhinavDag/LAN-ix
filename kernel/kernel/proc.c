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

struct pcb* create_process(void (*entry)(), int pid) {
    struct pcb* p = &processes[pid];

    static struct context contexts[MAX_PROCESSES];
    p->ctx = &contexts[pid];

    // Allocate stack
    static uint32_t stacks[MAX_PROCESSES][1024];
    uint32_t *stack = stacks[pid] + 1024;

    *(--stack) = (uint32_t)entry; // fake return address

    p->ctx->esp = (uint32_t)stack;

    p->state = PROC_RUNNABLE;
    p->tickets = 10;

    lottery_add_process(p);

    return p;
}
