CC = gcc
LD = ld
CFLAGS = -m32\
	-ffreestanding\
	-O2\
	-Wall\
	-Wextra\
	-fno-builtin\
	-nostdlib\
	-nodefaultlibs

LDFLAGS = -m elf_i386\
	-T link.ld\

.PHONY: all clean run iso # always treat these as commands

all: os.iso # build the bootable iso

kernel/boot/boot.o: kernel/boot/boot.S 
	$(CC) $(CFLAGS) -m32 -c kernel/boot/boot.S -o kernel/boot/boot.o

kernel/kernel/kernel.o: kernel/kernel/main.c
	$(CC) $(CFLAGS) -m32 -c kernel/kernel/main.c -o kernel/kernel/kernel.o

# link both into single ELF kernel
kernel.elf: kernel/boot/boot.o kernel/kernel/kernel.o link.ld
	$(LD) $(LDFLAGS) -o kernel.elf kernel/boot/boot.o kernel/kernel/kernel.o


kernel/boot/grub/grub.cfg: kernel.elf
	mkdir -p kernel/boot/grub
	cp kernel.elf kernel/boot/
	cp kernel/boot/grub.cfg.template kernel/boot/grub/grub.cfg

os.iso: kernel.elf kernel/boot/grub/grub.cfg
	grub-mkrescue -o os.iso kernel/  || \
		( echo "grub-mkrescue fail" && exit 1 )

run: os.iso
	qemu-system-i386 -cdrom os.iso -display gtk

clean:
	rm -rf kernel/boot/*.o kernel/kernel/*.o *.elf os.iso
	rm -rf kernel/boot/grub
	rm kernel/boot/*.elf
