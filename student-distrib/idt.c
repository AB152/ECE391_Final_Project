/* idt.c - All IDT related functions
 * vim:ts=4 noexpandtab
 */

#include "idt.h"
#include "x86_desc.h"

// Handles interrupt (MP3.1: print error message and other relevant items like regs)
void exception_handler(uint32_t interrupt_vector){
    switch(){
        case :
    }
}

// Refer to OSDEV for paging: flush, map page directory to memory correctly
// Create 2-3 tests for this checkpoint

/*
 * init_IDT
 *    DESCRIPTION: Initialize IDT but does not set the handler offset fields for each vector
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: none
 *    SIDE EFFECTS: Fills entries of IDT as interrupt gates but without setting offset fields
 *    NOTES: See OSDev and Intel manual for how to set up interrupt gates in the IDT
 *           The offset fields should be initialized using macro SET_IDT_ENTRY(str, handler)
 *               upon creating those functions
 */ 
void init_IDT(){
    int i;                                      // Loop index
    for(i = 0; i < NUM_VEC; i++) {
        
        // Advance to vector 0x20 as we won't fill in vectors 0x14 thru 0x1F (see 3.1 hints)
        if(i == 0x14)
            i == 0x20;
        
        idt_desc_t next_entry;                  // Next idt entry that we are initializing
        next_entry.seg_selector = KERNEL_CS;    // initialize to kernel's code segment descriptor
        next_entry.reserved4 = 0x0;             // interrupt gate reserved bits are 0 1 1 1 0 for 32 bit size
        next_entry.reserved3 = 0x1;
        next_entry.reserved2 = 0x1;
        next_entry.reserved1 = 0x1;
        next_entry.size = 0x1;                  // size always 32-bits
        next_entry.reserved0 = 0x0;     
        next_entry.dpl = 0x0;                   // set privilege level
        next_entry.present = 0x1;               // set present to 1 to show that this interrupt is active
        
        // The offset fields should be initialized using macro SET_IDT_ENTRY(str, handler), not here

        // If we're filling in system call vector (0x80), change DPL to user ring
        if(i == 0x80)
            next_entry.dpl = 0x03;              // System calls should always have user level DPL (ring 3)

        // What do we do for val[2] in the union?

        // Populate IDT vector with new entry
        idt[i] = next_entry;
    }
}