#include <stdint.h>
#include <stddef.h>
#include "gdt.h"

void kernel_main(void)
{
    gdt_init();
    // affichage
}

