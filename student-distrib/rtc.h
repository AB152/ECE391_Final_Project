
// https://wiki.osdev.org/RTC#Setting_the_Registers
// https://wiki.osdev.org/RTC#Turning_on_IRQ_8
// https://courses.engr.illinois.edu/ece391/sp2021/secure/references/ds12887.pdf slide 19 for RTC frequency
#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "types.h"

#define RTC_PORT		    0x70
#define RTC_IRQ             0x08
#define CMOS_PORT		    0x71
#define DISABLE_NMI_A	    0x8A
#define DISABLE_NMI_B	    0x8B
#define DISABLE_NMI_C	    0x8C
#define REGISTER_A		    0x0A
#define REGISTER_B		    0x0B
#define REGISTER_C		    0x0C

// RTC interrupt flag
volatile int RTC_int; 

// Initialize the RTC and turn on IRQ8
void init_RTC();

// Initialize RTC to 2 Hz
int RTC_open();

// Blocks system until RTC interrupt occurs
int RTC_read();

// Sets RTC frequency
int RTC_write(uint32_t * buf);

// Probably does nothing until we virtualize RTC
int RTC_close();

#endif /* _RTC_H */
