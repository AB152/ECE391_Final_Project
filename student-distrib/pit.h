#ifndef _PIT_H
#define _PIT_H

#include "lib.h"
#include "types.h"

/*
 *   Helpful PIT links:
 *      https://wiki.osdev.org/Programmable_interval_timer
 */

#define PIT_PORT		    0x70
#define PIT_IRQ             0x00
#define PIT_CH0             0x40
#define PIT_MODE_REG        0x43
#define PIT_FREQ            47727       // 1193180/25Hz(40ms) for frequency
#define PIT_MODE_2          0x34

// Initialize the RTC and turn on IRQ8
void init_PIT();

// Handles interrupts from the real-time clock
extern void PIT_handler();

#endif /* _PIT_H */
