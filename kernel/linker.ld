ENTRY(_start)

SECTIONS
{
    /* Load kernel at 1MB virtual/physical address */
    . = 1M;

    /* Executable Code & Multiboot Header */
    .text BLOCK(4K) : ALIGN(4K)
    {
        KEEP(*(.multiboot))   /* KEEP prevents garbage collection (ld --gc-sections) */
        KEEP(*(.multiboot2))
        *(.text*)
    }

    /* Read-Only Data (Constants, String Literals) */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata*)
    }

    /* Initialized Read-Write Data */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data*)
    }

    /* Uninitialized Read-Write Data (Zero-filled by bootloader/kernel) */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }

    /* Align heap boundary to 4KiB page size */
    . = ALIGN(4096);
    
    /* Exported Heap Start Symbol for Waks Arena Allocator */
    _waks_heap_start = .;
}
