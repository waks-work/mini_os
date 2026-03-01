/* boot/boot.s */
.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00
.long -(0x1BADB002 + 0x00)

.section .bss
.align 16
stack_bottom:
.skip 16384 
stack_top:

.section .text
.global _start
_start:
	mov $stack_top, %esp /* Set up the stack pointer */
        call kernel_main  /* Call the C kernel */
halt:
	hlt
	jmp halt
