/* kernel/kernel/pic.c */
#include <stdint.h>
#include "../include/timer.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret; __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port)); return ret;
}

/* Remap PIC to vectors 0x20-0x2F (32..47) */
void pic_remap(void) {
    uint8_t a1 = inb(0x21); // master PIC mask
    uint8_t a2 = inb(0xA1); // slave PIC mask

    outb(0x20, 0x11); // start init sequence (cascade)
    outb(0xA0, 0x11);
    outb(0x21, 0x20); // master offset 0x20 (32)
    outb(0xA1, 0x28); // slave offset 0x28 (40)
    outb(0x21, 0x04); // tell master there is a slave at IRQ2
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // restore saved masks
    outb(0x21, a1);
    outb(0xA1, a2);
}

/* unmask timer (IRQ0) — optional helper */
void pic_unmask_timer(void) {
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 0); // clear bit 0 => unmask IRQ0
    outb(0x21, mask);
}

/* send EOI for IRQ n (0..15) */
void pic_send_eoi(int irq) {
    if (irq >= 8) outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

