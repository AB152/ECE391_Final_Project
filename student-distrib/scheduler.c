#include "scheduler.h"
#include "system_calls.h"
#include "terminal.h"

/*
 * scheduler
 *    DESCRIPTION: Performs process switching
 *    INPUTS: none
 *    OUTPUTS: none
 *    SIDE EFFECTS: Switches active process
 */

void scheduler(){   // 
    pcb_t* curr_pcb = terminals[curr_terminal].terminal_pcb;
    
}
