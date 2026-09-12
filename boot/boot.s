.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00
.long -(0x1BADB002 + 0x00)

.section .bss
.align 4096
pml4_table:
.skip 4096
pdpt_table:
.skip 4096
pd_table:
.skip 4096

stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function

# GDT
.section .rodata
.align 8
gdt64:
    .quad 0x0000000000000000   # entry 0: null descriptor (required, always)
gdt64_code:
    .quad 0x00209A0000000000   # entry 1: 64-bit code segment
gdt64_end:

gdt64_pointer:
    .word gdt64_end - gdt64 - 1   # limit = size of GDT - 1
    .long gdt64                   # base = address of GDT

# IDT
.global isr0

# LIDT 
.global waks_lidt

.code32
_start:
    #enable PAE 
    mov %cr4, %eax 
    or  $0x20, %eax  # bit 5 = 0x20
    mov %eax, %cr4

    # add_pml4 to cr3
    mov $pml4_table, %eax
    mov %eax, %cr3

    # PML4[0] = address of PDPT | Present | Writable
    mov $pdpt_table, %eax
    or  $0x03, %eax          # bits 0,1 = Present, R/W
    mov %eax, (pml4_table)
    
    # PDPT[0] = address of PD | Present | Writable
    mov $pd_table, %eax
    or  $0x03, %eax
    mov %eax, (pdpt_table)
    
    # PD[0] = 0x000000 (physical addr 0) | Present | Writable | PS (2MB page)
    mov $0x83, %eax           # bits 0,1,7 = Present, R/W, PS
    mov %eax, (pd_table)

    # Enable long mode via EFER MSR 
    mov $0xC0000080, %ecx   # EFER MSR number
    rdmsr                    # read current EFER into EDX:EAX
    or  $0x100, %eax         # set bit 8 (LME) in the low 32 bits
    wrmsr                    # write EDX:EAX back into EFER

    #enable paging
    mov %cr0, %eax 
    or  $0x80000001, %eax
    mov %eax, %cr0

    # load the 64 bit GDT
    lgdt (gdt64_pointer)

    # far jumpt ot the 64 bit code segment (selector 0x08 = entry 1, index*8)
    ljmp $0x08, $long_mode_start

.code64 
long_mode_start:
    # Set up 16-byte aligned stack frame
    movq $stack_top, %rsp
    andq $-16, %rsp

    # Call the kernel C code
    call kernel_main

isr0:
    cli
    hlt
    iretq

waks_lidt: 
    lidt (%rdi)
    ret

halt:
    cli
    hlt
    jmp halt
