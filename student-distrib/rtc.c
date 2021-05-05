/* rtc.c - implementation of real time clock device
 *  vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "asm_linkage.h"
#include "x86_desc.h"
#include "i8259.h"

/* List of usable RTC frequencies as bitmaps to Register A's lowest 4 bits
   Ordered as 2^(index + 1) Hz but we don't go above 1024 Hz*/
unsigned char freq_list[10] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06};

// Used for date printing (3 chars for day + 1 char for NULL terminator)
char days_of_week[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; 
char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

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

    //cli();                              // important that no interrupts happen (perform a CLI)
    outb(DISABLE_NMI_B, RTC_PORT);		// select register B, and disable NMI
    uint32_t prev = inb(CMOS_PORT);	        // read the current value of register B
    outb(DISABLE_NMI_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS_PORT);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ);
    SET_IDT_ENTRY(idt[0x28], &RTC_processor);             //index 28 of IDT reserved for RTC
    RTC_int = 0;                // Clear RTC flag
    //sti();                      // (perform an STI) and reenable NMI if you wish? 

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
int32_t RTC_open(const uint8_t * filename) {

    // need to change frequency to 2Hz

    cli();
    outb(REGISTER_A, RTC_PORT);
    uint32_t prev = inb(CMOS_PORT); // read current value of register 
    outb(REGISTER_A, RTC_PORT);
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
int32_t RTC_read(int32_t fd, void * buf, int32_t n_bytes) {
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
int32_t RTC_write(int32_t fd, const void * buf, int32_t n_bytes) {
    uint32_t freq;          // Hold desired RTC frequency
    uint8_t index;          // Holds index of where the 1 bit is in the buffer
    uint8_t flag = 0;       // Checks how many bits of input buffer are set

    // Check null pointer
    if(buf == 0)
        return -1;

    // Read buffer and ignore LSB as frequency must be power of 2 excluding 1
    freq = *((uint8_t*)buf);
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
    outb(REGISTER_A, RTC_PORT);
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
int32_t RTC_close(int32_t fd) {
    return 0;
}

// EC: Prints the current CMOS time (Credit to: https://wiki.osdev.org/CMOS, but their example code was buggy)
void print_current_time() {
    unsigned char second, minute, hour, day_week, day_month, month;
    unsigned char prev_second = 0, prev_minute = 0, prev_hour = 0, prev_day_week = 0, prev_day_month = 0, prev_month = 0;
    unsigned int year;
    unsigned int prev_year = 0;
    unsigned char register_B_val;
    char day_week_string[4];    // 3 chars for string, 1 more for NULL terminator
    char month_string[4];
    memset(day_week_string, 0, 4);
    memset(month_string, 0, 4);

    // Read the date and time registers from CMOS/RTC
    second = get_RTC_register(0x00);    // The second register holds a value from 0-59
    minute = get_RTC_register(0x02);    // The minute register holds a value from 0-59
    hour = get_RTC_register(0x04);      // The hour register holds a value from 0-12 or 0-23 depending on mode
    day_week = get_RTC_register(0x06);  // The weekday register holds a value from 1-7, denoting day of the week
    day_month = get_RTC_register(0x07); // The day of month register holds a value from 1-31
    month = get_RTC_register(0x08);     // The month register holds a value from 1-12
    year = get_RTC_register(0x09);      // The year register only holds a value from 0-99

    // Use the "Read registers until consistent" method
    do {
        prev_second = second;
        prev_minute = minute;
        prev_hour = hour;
        prev_day_week = day_week;
        prev_day_month = day_month;
        prev_month = month;
        prev_year = year;
        while(get_update_in_progress_flag()) {
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day_week = get_RTC_register(0x06);
            day_month = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
        }
    } while(prev_second != second || prev_minute != minute || prev_hour != hour || prev_day_week != day_week
            || prev_day_month != day_month || prev_month != month || prev_year != year);

    // Grab (control) register B's value and check for various flags below
    register_B_val = get_RTC_register(REGISTER_B);

    // Convert Binary Coded Decimal to binary values if necessary (0x04 is the bit that denotes BCD)
    if(!(register_B_val & 0x04)) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
        day_month = (day_month & 0x0F) + ((day_month / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
    }

    // Convert 12-hr clock to 24-hr clock if not already 24-hr (0x02 is the bit that denotes 12 or 24 hr format)
    if(!(register_B_val & 0x02) && (hour & 0x80))
        hour = ((hour & 0x7F) + 12) % 24;

    // Add century offset to the year register to get current year
    year += 2000;

    // Convert day_week and month to get their strings
    memcpy(day_week_string, days_of_week[day_week - 1], 4);
    memcpy(month_string, months[month - 1], 4);

    // Print current time string in UTC
    printf("Current time: %s %s %d %d %d:%d:%d UTC\n", day_week_string, month_string, day_month, year, hour, minute, second);

    // Convert to CDT (UTC-5)
    hour = (hour - 5);
    // If we underflow (due to unsigned, it will underflow somewhere around 255)
    if(hour >= 24) {
        hour = hour - 232;  // Subtracting by 232 (255 - 23) allows us to realign with the bounds of 24-hr clock
        day_month--;  // I'm too lazy to check if this was 1 before decrement. Too many things to check :shrug:
        day_week--;
        // Decrementing Monday (1) should go to Saturday (7)
        if(day_week == 0)
            day_week = 7;
    }

    // Convert day_week and month to get their strings
    memcpy(day_week_string, days_of_week[day_week - 1], 4);
    memcpy(month_string, months[month - 1], 4);

    // Print current time string in CDT
    printf("              %s %s %d %d %d:%d:%d CDT\n", day_week_string, month_string, day_month, year, hour, minute, second);
}

// EC: Helper functions for getting current time from CMOS (Credit to: https://wiki.osdev.org/CMOS)
int get_update_in_progress_flag() {
      outb(REGISTER_A, RTC_PORT);
      return (inb(CMOS_PORT) & 0x80);
}

// EC: Helper functions for getting current time from CMOS (Credit to: https://wiki.osdev.org/CMOS) 
unsigned char get_RTC_register(int reg) {
      outb(reg, RTC_PORT);
      return inb(CMOS_PORT);
}
