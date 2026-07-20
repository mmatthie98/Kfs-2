.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    mov 4(%esp), %eax		# pointer to gdt
    lgdt (%eax)

    mov $0x10, %ax          # kernel data
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov $0x18, %ax          # kernel stack
    mov %ax, %ss

    ljmp $0x08, $flush_cs

flush_cs:
    ret
