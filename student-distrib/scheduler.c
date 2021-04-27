#include "scheduler.h"
#include "system_calls.h"
#include "terminal.h"
#include "i8259.h"
#include "pit.h"
#include "paging.h"
#include "x86_desc.h"


int count=0;
/*
 * scheduler
 *    DESCRIPTION: Performs process switching
 *    INPUTS: none
 *    OUTPUTS: none
 *    SIDE EFFECTS: Switches active process
 */

void scheduler(){   
    // Get the current active process's pid
    //int32_t curr_pid = terminals[curr_terminal].last_assigned_pid;
    pcb_t curr_pcb;
        
    // if(curr_pid==-1){   //takes care of booting up all three terminals
    if(count<3){
        curr_terminal++;
        asm volatile(       //initialize terminal esp and ebp before executing shell
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r"(curr_pcb.parent_esp), "=r"(curr_pcb.parent_ebp) // Outputs
        );
        count++;
        terminals[curr_terminal].terminal_pcb = &curr_pcb;
        terminals[curr_terminal].last_assigned_pid = curr_terminal;   //mark terminal as booted and initialize its pid
        execute((uint8_t *)"shell");
    }

    asm volatile(       //saving old process's stack to transfer over to new process's stack
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(terminals[curr_terminal].terminal_pcb->parent_esp), "=r"(terminals[curr_terminal].terminal_pcb->parent_ebp) // Outputs
    );

    // Round-robin increment
    curr_terminal = (curr_terminal + 1) % MAX_TERMINALS;

    pcb_t * next_pcb = terminals[curr_terminal].terminal_pcb;

    
    
    

    // Restore paging 
    set_user_video_page(1);
    set_user_prog_page(next_pcb->process_id, 1);

    // Update TSS
    tss.esp0 = EIGHT_MB - (next_pcb->process_id * EIGHT_KB) - 4;
    tss.ss0 = KERNEL_DS;
    
    asm volatile(       //Switch to next process's stack
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        : 
        :"r"(next_pcb->parent_esp), "r"(next_pcb->parent_ebp) // Inputs
    );
}
