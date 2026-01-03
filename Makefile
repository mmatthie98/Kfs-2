NAME    = kernel.bin
ISO     = kfs.iso
BOOT    = boot

CSRCS   = kernel.c gdt.c
ASRCS   = boot.s gdt_flush.s

OBJS    = $(CSRCS:.c=.o) $(ASRCS:.s=.o)

CC      = gcc
LD      = ld
RM      = rm -rf

CFLAGS  = -m32 -ffreestanding -fno-builtin -fno-stack-protector \
          -nostdlib -nodefaultlibs -Wall -Wextra -c

LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(CC) -m32 -c $< -o $@

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) $(ISO) $(BOOT)

re: fclean all

test: $(NAME)
	qemu-system-i386 -kernel $(NAME)

iso: $(NAME)
	mkdir -p $(BOOT)/grub
	cp $(NAME) $(BOOT)
	echo 'set timeout=0' > $(BOOT)/grub/grub.cfg
	echo 'set default=0' >> $(BOOT)/grub/grub.cfg
	echo 'menuentry "kfs2" {' >> $(BOOT)/grub/grub.cfg
	echo '  multiboot /$(NAME)' >> $(BOOT)/grub/grub.cfg
	echo '}' >> $(BOOT)/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(BOOT)

run: iso
	qemu-system-i386 -cdrom $(ISO)

.PHONY: all clean fclean re test iso run
