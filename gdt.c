#include "kernel.h"

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access_byte;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct gdt_entry *gdt = (struct gdt_entry *)GDT_ADDR;
static struct gdt_ptr *gdtp = (struct gdt_ptr *)(GDT_ADDR + GDT_ENTRIES * sizeof(struct gdt_entry));

extern void gdt_flush(uint32_t);

static void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access_byte, uint8_t gran)
{
    gdt[num].base_low    = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;
    gdt[num].limit_low   = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access_byte = access_byte;
}

void gdt_init(void)
{
    gdtp->limit = (GDT_ENTRIES * sizeof(struct gdt_entry)) - 1;
    gdtp->base = GDT_ADDR;

    gdt_set_entry(0, 0, 0, 0, 0);                  // null descriptor
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);   // kernel code
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);   // kernel data
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0x96, 0xCF);   // kernel stack
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xFA, 0xCF);   // user code
    gdt_set_entry(5, 0, 0xFFFFFFFF, 0xF2, 0xCF);   // user data
    gdt_set_entry(6, 0, 0xFFFFFFFF, 0xF6, 0xCF);   // user stack

    gdt_flush((uint32_t)gdtp);
}
