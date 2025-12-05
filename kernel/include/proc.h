#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include "lottery.h"

struct pcb* get_current_pcb(void);
void set_current_pcb(struct pcb* p);

struct pcb* create_process(void (*entry)(), int pid);
void init_process_system();

#endif
