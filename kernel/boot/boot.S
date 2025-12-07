/* kernel/boot/boot.S - minimal, safe */
.section .multiboot_header,"a"
.align 4

/* Multiboot magic, flags=0, checksum */
.long 0x1BADB002
.long 0x00000000
.long -(0x1BADB002 + 0x00000000)

.section .text.startup,"ax"
.code32
.global _start
.type _start, @function

_start:
    cli
    call kernel_main

hang:
    hlt
    jmp hang

.size _start, .-_start

.global load_idt
.type load_idt, @function
load_idt:
    mov 4(%esp), %eax 
    lidt (%eax)
    ret
