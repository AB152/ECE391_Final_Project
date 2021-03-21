/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

// NOTE THAT THE QEMU VM IS ONLY ONE SINGLE-CORE PROCESSOR (No locks needed, just CLI/STI) 

/* Interrupt masks to determine which interrupts are enabled and disabled (save what's currently masked) */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    // Clear interrupt flag so no interrupts occur during init
    cli();

    // Preserve original PIC masks
    master_mask = (uint8_t)inb(MASTER_8259_PORT + 1);
    slave_mask = (uint8_t)inb(SLAVE_8259_PORT + 1);

    // MAGIC NUMBERS and waiting using outb_p?

    outb(0xFF, MASTER_8259_PORT + 1);           // Mask all of 8259A-1
    outb(0xFF, SLAVE_8259_PORT + 1);            // Mask all of 8259A-2

    outb(ICW1, MASTER_8259_PORT);               // ICW1: select 8259A-1 init
    outb(ICW2_MASTER, MASTER_8259_PORT + 1);    // ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27
    outb(ICW3_MASTER, MASTER_8259_PORT + 1);    // 8259A-1 has slave on IR2

    outb(ICW4, MASTER_8259_PORT + 1);           // master expects normal EOI

    outb(ICW1, SLAVE_8259_PORT);                // ICW1: select 8259A-2 init
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);      // ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2F
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);      // 8259A-2 is slave on master's IR2
    outb(ICW4, SLAVE_8259_PORT + 1);            // (slave's support for AEOI in flat more is to be investigated)

    // Enable port of master that slave is connected to 
    outb(master_mask, MASTER_8259_PORT + 1);   // restore master IRQ mask
    outb(slave_mask, SLAVE_8259_PORT + 1);   // restore slave IRQ mask

    // Initialization complete: re-allow interrupts
    sti();
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint8_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = MASTER_8259_PORT + 1;
    } else {
        port = SLAVE_8259_PORT + 1;
        irq_num -= 8;
    }
    value = inb(port) | (1 << irq_num);
    outb(port, value); 
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint8_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = MASTER_8259_PORT + 1;
    } else {
        port = SLAVE_8259_PORT + 1;
        irq_num -= 8;
    }
    value = inb(port) & ~(1 << irq_num);
    outb(port, value); 
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {

    // Should EOI be 0x20 and not 0x60?

	if(irq_num >= 8)
		outb(SLAVE_8259_PORT, EOI);
 
	outb(MASTER_8259_PORT, EOI);
}
