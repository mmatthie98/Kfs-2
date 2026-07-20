NAME	= kernel.bin
CSRCS	= kernel.c gdt.c
ASRCS	= boot.s gdt_flush.s
ISO		= kfs.iso

OBJS	= $(CSRCS:.c=.o) $(ASRCS:.s=.o)

CC	= gcc
LD	= ld
RM	= rm -rf

CFLAGS	= -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -ffreestanding -c
ASFLAGS	= -m32 -c
LDFLAGS	= -m elf_i386 -T linker.ld

%.o: %.c
			$(CC) $(CFLAGS) $< -o $@

%.o: %.s
			$(CC) $(ASFLAGS) $< -o $@

$(NAME):	$(OBJS)
			$(LD) $(LDFLAGS) $(OBJS) -o $(NAME)

all:		$(NAME)

clean:
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME) iso

re:			fclean all

run:		$(ISO)
			qemu-system-i386 -cdrom $(ISO)

test:		$(NAME)
			qemu-system-i386 -kernel $(NAME)

iso:		$(NAME)
			mkdir -p iso/boot/grub
			cp $(NAME) iso/boot
			echo 'menuentry "kfs" {\n\tmultiboot /boot/$(NAME)\n}' > iso/boot/grub/grub.cfg
			grub-mkrescue -o $(ISO) iso

.PHONY:		all clean fclean re run test iso
