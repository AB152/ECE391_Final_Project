/* rtc.c - implementation of real time clock device
 *  vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "idt_ex_handler.h"
#include "x86_desc.h"
#include "i8259.h"

/* List of usable RTC frequencies as bitmaps to Register A's lowest 4 bits
   Ordered as 2^(index + 1) Hz but we don't go above 1024 Hz*/
unsigned char freq_list[10] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06};

/*
 * init_RTC
 *    DESCRIPTION: Initialize RTC and turns on IRQ8
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: none
 *    SIDE EFFECTS: Turns on periodic interupts
 *    NOTES: See OSDev links in .h file to understand macros
 */ 
void init_RTC() {

    cli();                              // important that no interrupts happen (perform a CLI)
    outb(DISABLE_NMI_B, RTC_PORT);		// select register B, and disable NMI
    uint32_t prev = inb(CMOS_PORT);	        // read the current value of register B
    outb(DISABLE_NMI_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS_PORT);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ);
    SET_IDT_ENTRY(idt[0x28], &RTC_processor);             //index 28 of IDT reserved for RTC
    RTC_int = 0;                // Clear RTC flag
    sti();                      // (perform an STI) and reenable NMI if you wish? 

}

/*
 * RTC_interrupt
 *    DESCRIPTION: RTC register C needs to be read, so interupts will happen again
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: none
 *    SIDE EFFECTS: set RTC_int to 1
 *    NOTES: See OSDev links in .h file to understand macros
 */ 
void RTC_interrupt(){
    outb(REGISTER_C, RTC_PORT);	    // select register C
    inb(CMOS_PORT);		            // just throw away contents
    RTC_int = 1;                    // RTC interupt has occured
    
    send_eoi(RTC_IRQ);
    
    // Possible space to put test_interrupts() function.
    //test_interrupts();
}

/*
 * RTC_open
 *    DESCRIPTION: Set RTC frequency to 2 Hz
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: Always returns 0
 *    SIDE EFFECTS: Sets RTC frequency to 2 Hz
 *    NOTES: See OSDev links in .h file to understand macros
 */
int RTC_open() {

    // need to change frequency to 2Hz

    cli();
    outb(REGISTER_A, RTC_PORT);
    uint32_t prev = inb(CMOS_PORT); // read current value of register 
    outb(prev | 0x0F, CMOS_PORT); // RS3 to RS0 must all be turned on for 2Hz = 0x0F
    sti();

    return 0;
}

/*
 * RTC_read
 *    DESCRIPTION: Blocks system until RTC interrupt
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: Always returns 0
 *    SIDE EFFECTS: Blocks system until RTC interrupt
 *    NOTES: none
 */
int RTC_read() {
    while(!RTC_int);    // Block until flag is set by interrupt handler
    RTC_int = 0;        // Reset RTC flag
    return 0;
}

/*
 * RTC_write
 *    DESCRIPTION: Changes frequency of RTC to input value
 *    INPUTS: buf -- input buffer that holds the new frequency
 *    OUTPUTS: none
 *    RETURNS: 0 if successful, -1 if input is not a power of 2 or is invalid
 *    SIDE EFFECTS: Changes RTC frequency to input value
 *    NOTES: Input cannot surpass 1024, check is at line 133
 */
int RTC_write(uint32_t * buf) {
    uint32_t freq;          // Hold desired RTC frequency
    uint8_t index;          // Holds index of where the 1 bit is in the buffer
    uint8_t flag = 0;       // Checks how many bits of input buffer are set

    // Check null pointer
    if(buf == 0)
        return -1;

    // What does "rtc_write must get its input parameter through a buffer and not read the value directly." mean???

    // Read buffer and ignore LSB as frequency must be power of 2 excluding 1
    freq = *buf;
    index = 0;
    
    // If LSB is set in the first place, it's invalid
    if(freq % 2)
        return -1;
    
    // Ignore LSB
    freq >>= 1;
    

    // If there is no bit set that is a power of 2 excluding 1, it's invalid
    if(freq == 0)
        return -1;

    // Check if input is power of 2
    while(freq){
        
        // Check if input is above 1024 (bit 10), if so, it's invalid
        if(index > 10)
            return -1;
        
        // Check if next bit is 1
        if(freq % 2)
            flag++;
        
        // If there isn't exactly one 1, it's not a power of 2 and is invalid
        if(flag > 1)
            return -1;

        // Increment index and advance
        index++;
        freq >>= 1;
    }

    // Clear interrupt flag - entering crit. section
    cli();

    // Get current value of register
    outb(REGISTER_A, RTC_PORT);
    uint32_t prev = inb(CMOS_PORT); 

    // Clear existing frequency bits
    prev &= 0xF0;

    // Set RTC to new frequency
    outb(prev | freq_list[index - 1], CMOS_PORT);

    // Re-allow interrupts and return
    sti();
    return 0;
}

/*
 * RTC_close
 *    DESCRIPTION: Probably does nothing unless we virtualize RTC
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: Always returns 0
 *    SIDE EFFECTS: none?
 *    NOTES: none
 */
int RTC_close() {
    return 0;
}
