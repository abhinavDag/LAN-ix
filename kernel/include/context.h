#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>

struct context {
    uint32_t esp;     // stack pointer
};

void context_switch(struct context *old, struct context *new);

#endif
