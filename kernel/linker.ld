ENTRY(_start)

SECTIONS
{
    . = 1M; /* standard starting point for kernels */
    .text BLOCK(4K): ALIGN(4K)
    {
         *(.multiboot) /* for grub compatibility */
         *(.text)
    }
    
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)     
    }
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)       
    }
    
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss) 
    }
}
