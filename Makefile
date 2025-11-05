CC = gcc
LD = ld
CFLAGS = -m32\
	-ffreestanding\
	-O2\
	-Wall\
	-Wextra\
	-fno-builtin\
	-nostdlib\
	-nodefaultlibs\
LDFLAGS = -m elf_i386\
	-T link.ld\

.PHONY: all clean run iso # always treat these as commands

all: os.iso # build the bootable iso

boot.o: boot.S 
	$(CC) -m32 -c boot.S -o boot.o

kernel.o: kernel.c
	$(CC) -m32 -c kernel.c -o kernel.o

# link both into single ELF kernel
kernel.elf: boot.o kernel.o link.ld
	$(LD) $(LDFLAGS) -o kernel.elf boot.o kernel.o


iso/boot/grub/grub.cfg: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/
	@cat > iso/boot/grub/grub.cfg <<'EOF'
		set timeout=0
		set default=0

		menuentry "lanix" {
			multiboot /boot/kernel.elf
			boot
		}
	EOF

os.iso: kernel.elf iso/boot/grub/grub.cfg
	grub-mkrescue -o os.iso iso 2>/dev/null || \
		( echo "grub-mkrescue fail" && exit 1 )

run: os.iso
	qemu-system-i386 -cdrom os.iso

clean:
	rm -f *.o *.elf os.iso
	rm -rf iso
