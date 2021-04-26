#include "scheduler.h"
#include "system_calls.h"
#include "terminal.h"
#include "i8259.h"
#include "pit.h"

/*
 * scheduler
 *    DESCRIPTION: Performs process switching
 *    INPUTS: none
 *    OUTPUTS: none
 *    SIDE EFFECTS: Switches active process
 */

void scheduler(){   // 
    // Get the current active process's PCB
    pcb_t* curr_pcb = terminals[curr_terminal].terminal_pcb;
    
    // All 3 terminals booted in kernel.c

    // Round-robin increment
    curr_terminal = (curr_terminal + 1) % MAX_TERMINALS;

}
