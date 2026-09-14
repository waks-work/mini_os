#include "pic.h"

void pic_remap(waks_uchar offset1, waks_uchar offset2)
{
    // Save current IRQ masks
    waks_uchar a1 = waks_bm_inb(PIC1_DATA);
    waks_uchar a2 = waks_bm_inb(PIC2_DATA);

    // ICW1: Start initialization sequence
    waks_bm_outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    waks_bm_io_wait();
    waks_bm_outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    waks_bm_io_wait();

    // ICW2: Set Master/Slave vector offsets
    waks_bm_outb(PIC1_DATA, offset1);
    waks_bm_io_wait();
    waks_bm_outb(PIC2_DATA, offset2);
    waks_bm_io_wait();

    // ICW3: Master/Slave cascade connection
    waks_bm_outb(PIC1_DATA, 4);
    waks_bm_io_wait();
    waks_bm_outb(PIC2_DATA, 2);
    waks_bm_io_wait();

    // ICW4: Set 8086 mode
    waks_bm_outb(PIC1_DATA, ICW4_8086);
    waks_bm_io_wait();
    waks_bm_outb(PIC2_DATA, ICW4_8086);
    waks_bm_io_wait();

    // Mask all interrupts for now
    waks_bm_outb(PIC1_DATA, 0xFF);
    waks_bm_outb(PIC2_DATA, 0xFF);
}

void pic_send_eoi(waks_uchar irq)
{
    if (irq >= 8) {
        waks_bm_outb(PIC2_COMMAND, 0x20);
    }
    waks_bm_outb(PIC1_COMMAND, 0x20);
}

void pic_disable(void) {
    waks_bm_outb(PIC1_DATA, 0xff);
    waks_bm_outb(PIC2_DATA, 0xff);
}


// masking and unmasking 
void irq_set_mask(uint8_t irqline) {
    uint16_t port;
    uint8_t value;

    if(irqline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqline -= 8;
    }
    value = waks_bm_inb(port) | (1 << irqline);
    waks_bm_outb(port, value);        
}

void irq_clear_mask(uint8_t irqline) {
    uint16_t port;
    uint8_t value;

    if(irqline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqline -= 8;
    }
    value = waks_bm_inb(port) & ~(1 << irqline);
    waks_bm_outb(port, value);        
}
