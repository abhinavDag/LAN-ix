#include <stdint.h>
#include "../include/interrupts.h"

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idt_descriptor;

extern void load_idt(const void *ptr);

static void set_gate(int n, uint32_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init() {
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base  = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt[i].offset_low  = 0;
        idt[i].selector    = 0x08;
        idt[i].zero        = 0;
        idt[i].type_attr   = 0x8E;
        idt[i].offset_high = 0;
    }

    extern void timer_isr_stub();
    set_gate(32, (uint32_t)timer_isr_stub);

    load_idt(&idt_descriptor);
}

