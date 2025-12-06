OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x00100000;

    .multiboot_header : {
        KEEP(*(.multiboot_header))
    }

    .text : {
        *(.text.startup)    /* force entry code here first */
        *(.text*)           /* everything else */
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }
}
