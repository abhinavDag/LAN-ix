	.section .multiboot
	.align 4

	.long 0x1BADB002
	.long 0x00000000
	.long -(0x1BADB002 + 0x00000000)

	.section .text
	.global start
	.type start, @function

start:
	cli
	call kernel_main

hang:
	cli
	hlt
	jmp .hang

.size start, .-start
