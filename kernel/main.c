#include <stdint.h>

// Simple I/O port write
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void qemu_printf(const char *s)
{
    for (int i = 0; s[i] != '\0'; i++)
        outb(0x3F8, s[i]); // send char to COM1
}

void kernel_main(void)
{
    unsigned short *terminal_buffer = (unsigned short *)0xB8000;
    const char *str = "mini_os project startup";

    for (int i = 0; str[i] != '\0'; i++)
    {
        terminal_buffer[i] = (unsigned short)str[i] | (0x0F << 8);
    }

    // Serial output (for your 'display none' terminal)
    qemu_printf("\n[WaksOS] Kernel Loaded Successfully.\n");
    qemu_printf("[WaksOS] Silicon Block: 6 Hours Remaining.\n");

    for (;;)
    {
        __asm__("hlt");
    }
}
