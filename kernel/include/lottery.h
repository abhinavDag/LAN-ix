#ifndef LOTTERY_H
#define LOTTERY_H

#include <stdint.h>

enum proc_state { PROC_UNUSED = 0, PROC_RUNNABLE, PROC_RUNNING, PROC_SLEEPING, PROC_ZOMBIE };

struct context;   // <-- forward declaration ONLY

struct pcb {
    int pid;
    enum proc_state state;
    struct context *ctx;
    uint32_t tickets;
    struct pcb *next;
};

/* Scheduler API */
void lottery_init(void);
void lottery_add_process(struct pcb *p);
void lottery_remove_process(struct pcb *p);
int lottery_set_tickets(int pid, uint32_t tickets);
int sys_set_tickets(uint32_t tickets);
void lottery_schedule_tick(void);
void lottery_schedule_yield(void);

#endif
