#include "waks.h"

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

// define our array of 256 interupt descriptors
static m_idt_entry idt[256];
static m_idt_ptr idtp;

// declared in boot.s
extern void isr0(void);
extern void waks_lidt(m_idt_ptr *ptr);

// helper to create a 16 byte long mode idt gate
void m_idt_set_gate(waks_uchar vector, waks_u64 isr_address, waks_u16 selector, waks_uchar flags) 
{
	idt[vector].offset_low      = (waks_u16)(isr_address & 0xFFFF);
	idt[vector].selector        = selector;
	idt[vector].ist             = 0;
	idt[vector].type_attributes = flags; // 0x8E: Present, DPL 0, 64-bit Interrupt Gate
	idt[vector].offset_mid      = (waks_u16)((isr_address >> 16) & 0xFFFF);
	idt[vector].offset_high     = (waks_u32)((isr_address >> 32) & 0xFFFFFFFF);
	idt[vector].zero            = 0;
}

void m_init_idt(void) 
{
    // zero out the idt
	for (waks_u32 i = 0; i < 256; i++) {
        idt[i] = (m_idt_entry){0};
    }
    
    // map vector 0 to assembly stub isr0, 0x08 = Kernel Code Segment, 0x8E = Interrupt Gate
    m_idt_set_gate(0, (waks_u64)isr0, 0x08, 0x8E);

    // construct and idt pointer 
    idtp.limit = (sizeof(m_idt_entry)* 256) - 1;
    idtp.base  = (waks_u64)&idt;
    waks_lidt(&idtp);
}
