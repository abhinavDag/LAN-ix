CC = gcc
LD = ld

CFLAGS = -m32 \
	-ffreestanding \
	-O2 \
	-Wall \
	-Wextra \
	-fno-builtin \
	-nostdlib \
	-nodefaultlibs \
	-Ikernel/include

LDFLAGS = -m elf_i386 \
	-T link.ld

.PHONY: all clean run iso

all: os.iso


# =====================================================
# Boot Code
# =====================================================

kernel/boot/boot.o: kernel/boot/boot.S
	$(CC) $(CFLAGS) -c kernel/boot/boot.S -o kernel/boot/boot.o


# =====================================================
# Kernel Core
# =====================================================

kernel/kernel/kernel.o: kernel/kernel/main.c
	$(CC) $(CFLAGS) -c kernel/kernel/main.c -o kernel/kernel/kernel.o

kernel/kernel/lottery_scheduler.o: kernel/kernel/lottery_scheduler.c kernel/include/lottery.h
	$(CC) $(CFLAGS) -c kernel/kernel/lottery_scheduler.c -o kernel/kernel/lottery_scheduler.o

kernel/kernel/proc.o: kernel/kernel/proc.c kernel/include/proc.h kernel/include/context.h
	$(CC) $(CFLAGS) -c kernel/kernel/proc.c -o kernel/kernel/proc.o

kernel/kernel/context_switch.o: kernel/kernel/context_switch.s
	$(CC) $(CFLAGS) -c kernel/kernel/context_switch.s -o kernel/kernel/context_switch.o

kernel/kernel/interrupts.o: kernel/kernel/interrupts.c kernel/include/interrupts.h
	$(CC) $(CFLAGS) -c kernel/kernel/interrupts.c -o kernel/kernel/interrupts.o

kernel/kernel/idt.o: kernel/kernel/idt.c kernel/include/interrupts.h
	$(CC) $(CFLAGS) -c kernel/kernel/idt.c -o kernel/kernel/idt.o

kernel/kernel/timer.o: kernel/kernel/timer.c kernel/include/timer.h
	$(CC) $(CFLAGS) -c kernel/kernel/timer.c -o kernel/kernel/timer.o

kernel/kernel/timer_isr.o: kernel/kernel/timer_isr.s
	$(CC) $(CFLAGS) -c kernel/kernel/timer_isr.s -o kernel/kernel/timer_isr.o

kernel/kernel/cmdline.o: kernel/kernel/cmdline.c
	$(CC) $(CFLAGS) -c kernel/kernel/cmdline.c -o kernel/kernel/cmdline.o


# =====================================================
# Link Kernel
# =====================================================

kernel.elf: \
	kernel/boot/boot.o \
	kernel/kernel/kernel.o \
	kernel/kernel/lottery_scheduler.o \
	kernel/kernel/proc.o \
	kernel/kernel/context_switch.o \
	kernel/kernel/interrupts.o \
	kernel/kernel/idt.o \
	kernel/kernel/timer.o \
	kernel/kernel/timer_isr.o \
	kernel/kernel/cmdline.o

	$(LD) $(LDFLAGS) -o kernel.elf \
		kernel/boot/boot.o \
		kernel/kernel/kernel.o \
		kernel/kernel/lottery_scheduler.o \
		kernel/kernel/proc.o \
		kernel/kernel/context_switch.o \
		kernel/kernel/interrupts.o \
		kernel/kernel/idt.o \
		kernel/kernel/timer.o \
		kernel/kernel/timer_isr.o \
		kernel/kernel/cmdline.o


# =====================================================
# ISO Build
# =====================================================

kernel/boot/grub/grub.cfg: kernel.elf
	mkdir -p kernel/boot/grub
	cp kernel.elf kernel/boot/
	cp kernel/boot/grub.cfg.template kernel/boot/grub/grub.cfg

os.iso: kernel.elf kernel/boot/grub/grub.cfg
	grub-mkrescue -o os.iso kernel/ || \
		( echo "grub-mkrescue fail" && exit 1 )


# =====================================================
# Run
# =====================================================

run: os.iso
	qemu-system-i386 -cdrom os.iso -display gtk


# =====================================================
# Clean
# =====================================================

clean:
	rm -rf kernel/boot/*.o kernel/kernel/*.o *.elf os.iso
	rm -rf kernel/boot/grub
	rm -f kernel/boot/*.elf
