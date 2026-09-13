#include "idt.h"

__attribute__((noinline)) void exception_handler(m_registers *regs)
{
    waks_io_print(WAKS_2STR("\n--- KERNEL PANIC: CPU EXCEPTION --- \n"));
    
    waks_io_print(WAKS_2STR("Vector: "));
    waks_io_print_u64(regs->vector_num);
    
    waks_io_print(WAKS_2STR(" | Error Code: "));
    waks_io_print_u64(regs->error_code);
    
    waks_io_print(WAKS_2STR("\nRIP: "));
    waks_io_print_u64(regs->rip);
    
    waks_io_print(WAKS_2STR(" | RSP: "));
    waks_io_print_u64(regs->rsp);
    waks_io_print(WAKS_2STR("\n"));

    // Halt system safely
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}
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
    m_idt_set_gate( 0,  (waks_u64)isr0, 0x08, 0x8E);
    m_idt_set_gate( 6,  (waks_u64)isr6, 0x08, 0x8E);
    m_idt_set_gate(13, (waks_u64)isr13, 0x08, 0x8E);
    m_idt_set_gate(14, (waks_u64)isr14, 0x08, 0x8E);

    // construct and idt pointer 
    idtp.limit = (sizeof(m_idt_entry)* 256) - 1;
    idtp.base  = (waks_u64)&idt;
    waks_lidt(&idtp);
}
