#define WAKS_TYPE_IMPLEMENTATION
#define WAKS_ARCH_IMPLEMENTATION
#define WAKS_ALLOCATOR_IMPLEMENTATION
#define WAKS_CONTAINER_IMPLEMENTATION
#define WAKS_IO_IMPLEMENTATION

#include "waks.h"
#include "idt.h"

void kernel_main(void)
{
    // Serial output (for your 'display none' terminal)
    waks_io_print(WAKS_2STR("\n[mini_OS] Kernel Loaded Successfully.\n"));

    m_init_idt();
    waks_io_print(WAKS_2STR("[mini_OS] IDT Loaded Successfully.\n"));

    waks_u64 a = 14;
    waks_u64 b =  0;
    waks_u64 number = a/b;

    // unreachable cpu halts on isr0
    waks_io_print_u64(number);

    for (;;) {
        __asm__("hlt");
    }
}
