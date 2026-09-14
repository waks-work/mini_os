#ifndef IDT_H 
#define IDT_H

#include "waks.h"

#ifdef __cplusplus
extern "C" {
#endif

// 16 byte long mode IDT gate descriptor
typedef struct __attribute__((packed)) {
    waks_u16    offset_low;      // offset bits 0..15
    waks_u16    selector;        // a code segment selector in GDT or LDT
    waks_uchar  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    waks_uchar  type_attributes; // gate type, dpl, and p fields
    waks_u16    offset_mid;      // offset bits 16..31
    waks_u32    offset_high;     // offset bits 32..63
    waks_u32    zero;            // reserved
} m_idt_entry;

// structure loaded via the lidt: simillar to gdt_64_pointer
typedef struct __attribute__((packed)) {
	waks_u16 limit;     // size of IDT - 1  so we get the correct size 
	waks_u64 base;      // 64 bit base address of the idt array
} m_idt_ptr;


typedef struct __attribute__((packed)) {
    // manually pushed to the stack 
    waks_u64 r15, r14, r13, r12, r11, r10, r9, r8;
    waks_u64 rdi, rsi, rbp, rbx, rdx, rcx, rax;

    // pushed by the isr macro  
    waks_u64 vector_num;
    waks_u64 error_code; // by the cpu for some error codes

    // pushed by the cpu 
    waks_u64  rip;
    waks_u64  cs;
    waks_u64  rflags;
    waks_u64  rsp;
    waks_u64  ss;
} m_registers;

// define our array of 256 interupt descriptors
extern m_idt_entry idt[256];
extern m_idt_ptr idtp;

// declared in boot.s
extern void waks_lidt(m_idt_ptr *ptr);

// isr specific definition
extern void isr0(void);
extern void isr6(void);
extern void isr13(void);
extern void isr14(void);

// idt
void m_idt_set_gate(waks_uchar vector, waks_u64 isr_address, waks_u16 selector, waks_uchar flags);
void m_init_idt(void);

// isr called in boot.s
__attribute__((noinline)) void exception_handler(m_registers *regs);

#ifdef __cplusplus 
}
#endif

#endif // IDT_H
