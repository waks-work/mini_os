#ifndef PIC_H
#define PIC_H

#include "waks.h"
#include "../thirdparty/waks_cstd/arch.h"

#ifdef __cpluspus 
extern "C" {
#endif

// 8259 ports
#define PIC1_COMMAND  0x20
#define PIC1_DATA	  0x21
#define PIC2_COMMAND  0xA0 
#define PIC2_DATA	  0xA1

// initialisation command flags
#define ICW1_ICW4	   0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	   0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	   0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	   0x10		/* Initialization - required! */

#define ICW4_8086       0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	    0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	    0x10		/* Special fully nested (not) */

// Vector Remap Base Limits (32 for Master, 40 for Slave)
#define PIC1_OFFSET     0x20    /* IRQ 0..7  -> IDT 32..39 */
#define PIC2_OFFSET     0x28    /* IRQ 8..15 -> IDT 40..47 */

// Public API
void pic_remap(waks_uchar offset1, waks_uchar offset2);
void pic_send_eoi(waks_uchar irq);
void pic_disable(void);


#ifdef __cpluspus 
}
#endif

#endif
