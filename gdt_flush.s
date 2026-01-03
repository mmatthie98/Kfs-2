.global gdt_flush

gdt_flush:
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $0x18, %ax      # kernel stack selector
    mov %ax, %ss

    mov $0x10, %ax      # kernel data selector
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    ljmp $0x08, $reload_cs
reload_cs:
    ret
