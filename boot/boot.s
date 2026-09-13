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
    mov $0xC0000080, %ecx    # EFER MSR number
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

waks_lidt: 
    lidt (%rdi)
    ret

# ISR MACROS STUBS 
.macro ISR_NOERRCODE num 
.global isr\num
isr\num:
    cli
    pushq $0         # dummy error code to keep stack uniform
    pushq $\num      # push the vector number
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE num 
.global isr\num
isr\num:
    cli
    # error code is sent by the hardware
    pushq $\num      # push the vector number
    jmp isr_common_stub
.endm

# Generate the stubs
ISR_NOERRCODE 0  # divide by zero error (#DE)
ISR_NOERRCODE 6  # invalid opcode (#UD)
ISR_ERRCODE   13 # general protection fault (#GP)
ISR_ERRCODE   14 # page fault error (#PF)

.global isr_common_stub
.extern exception_handler

isr_common_stub:
    # Save general-purpose registers (System V ABI context)
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rbx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    # Pass pointer to the saved stack frame as 1st argument (%rdi) to C
    movq %rsp, %rdi
    call exception_handler

    # Restore general-purpose registers
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax

    # Clean up vector number and error code (16 bytes)
    addq $16, %rsp

    # Return from interrupt (restores CS, RIP, RFLAGS, RSP, SS)
    iretq

halt:
    cli
    hlt
    jmp halt
