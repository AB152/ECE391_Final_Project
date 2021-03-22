
// https://wiki.osdev.org/RTC#Setting_the_Registers
// https://wiki.osdev.org/RTC#Turning_on_IRQ_8
#include "lib.h"


#define RTC_PORT		    0x70
#define RTC_IRQ             0x08
#define CMOS_PORT		    0x71
#define DISABLE_NMI_A	    0x8A
#define DISABLE_NMI_B	    0x8B
#define DISABLE_NMI_C	    0x8C
#define REGISTER_A		    0x0A
#define REGISTER_B		    0x0B
#define REGISTER_C		    0x0C

//volatile int RTC_int; // to keep track of RTC interupts

// Initialize the RTC and turn on IRQ8
void init_RTC();
