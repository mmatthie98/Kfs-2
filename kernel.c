#include "kernel.h"

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}

void terminal_write_hex(uint32_t value)
{
	char hex[] = "0123456789ABCDEF";
	char buffer[11];

	buffer[0] = '0';
	buffer[1] = 'x';

	for (int i = 0; i < 8; i++) {
		buffer[9 - i] = hex[value & 0xF];
		value >>= 4;
	}

	buffer[10] = '\0';
	terminal_writestring(buffer);
}

static inline uint32_t get_esp(void)
{
	uint32_t esp;
	__asm__ volatile ("mov %%esp, %0" : "=r"(esp));
	return esp;
}

void terminal_newline(void)
{
	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT)
		terminal_row = 0;
}

void terminal_writestring_line(const char* str)
{
	terminal_writestring(str);
	terminal_newline();
}

void print_kernel_stack(uint32_t entries)
{
	uint32_t *esp = (uint32_t *)get_esp();

	terminal_writestring("Kernel stack address (ESP) : ");
	terminal_write_hex((uint32_t)esp);
	terminal_newline();
	terminal_newline();

	for (uint32_t i = 0; i < entries; i++) {
		terminal_writestring("Addr : [");
		terminal_write_hex((uint32_t)(esp + i));
		terminal_writestring("] - Data : ");
		terminal_write_hex(esp[i]);
		terminal_newline();
	}
}

void check_gdt(void)
{
    struct __attribute__((packed)) {
        uint16_t limit;
        uint32_t base;
    } gdtr;

    __asm__ volatile (
        "sgdt %0"
        : "=m"(gdtr)
    );

    terminal_writestring("GDT base  : ");
    terminal_write_hex(gdtr.base);
    terminal_newline();

    terminal_writestring("GDT limit : ");
    terminal_write_hex(gdtr.limit);
    terminal_newline();

    if (gdtr.base == 0x00000800) {
        terminal_writestring("GDT address OK!");
    } else {
        terminal_writestring("ERROR: GDT is not at 0x00000800!");
    }

    terminal_newline();
}

void kernel_main(void)
{
    terminal_initialize();

    terminal_writestring_line("BEFORE GDT INIT");

    gdt_init();

    terminal_writestring_line("AFTER GDT INIT");

    check_gdt();

    terminal_writestring_line("AFTER GDT CHECK");

    terminal_newline();

    print_kernel_stack(8);
}
