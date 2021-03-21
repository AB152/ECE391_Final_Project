
#include "rtc.h"
#include "i8259.h"


/*
 * init_RTC
 *    DESCRIPTION: Initialize RTC and turns on IRQ8
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: none
 *    SIDE EFFECTS: Turns on periodic interupts
 *    NOTES: See OSDev links in .h file to understand macros
 */ 

void init_RTC(){

    //cli();                              // important that no interrupts happen (perform a CLI)
    outb(DISABLE_NMI_B, RTC_PORT);		// select register B, and disable NMI
    uint32_t prev = inb(CMOS_PORT);	        // read the current value of register B
    outb(DISABLE_NMI_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS_PORT);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ);
    //sti();                      // (perform an STI) and reenable NMI if you wish? 

}

