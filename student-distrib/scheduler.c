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
    int i;
    // Get the current active process's pid
    int32_t curr_pid = terminals[curr_terminal].current_pid;
                 
    if(terminals[i].current_pid==-1){   //takes care of booting up all three terminal




        execute((uint8_t *)"shell");
        terminals[i].current_pid = last_assigned_pid;   //mark terminal as booted and initialize its pid
    }

    // Round-robin increment
    curr_terminal = (curr_terminal + 1) % MAX_TERMINALS;

}
