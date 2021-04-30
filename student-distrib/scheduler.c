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
 *    SIDE EFFECTS: Switches active process using round-robin scheduling
 */
void scheduler(){   
    // Get the current active process's PCB
    pcb_t * curr_pcb = terminals[curr_terminal].terminal_pcb;
        
    // Boot up the three terminals
    if(curr_pcb == NULL && count < 3){
        pcb_t temp_pcb;
        asm volatile(       // Retrieve initial ESP and EBP before booting base shells
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r"(temp_pcb.curr_esp), "=r"(temp_pcb.curr_ebp) // Outputs
        );
        count++;
        terminals[curr_terminal].terminal_pcb = &temp_pcb;
        terminals[curr_terminal].last_assigned_pid = curr_terminal;   //mark terminal as booted and initialize its pid
        //terminals[curr_terminal].terminal_pcb->parent_pcb = (pcb_t *)(EIGHT_MB - ((count + 1) * EIGHT_KB));
        switch_visible_terminal(curr_terminal);
        printf("Terminal %d booting...\n", count);
        execute((uint8_t *)"shell");
    }

    // Save old process's stack (also properly sets up first base shell's ESP/EBP)
    asm volatile(       
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(curr_pcb->curr_esp), "=r"(curr_pcb->curr_ebp) // Outputs
    );

    // Round-robin increment
    curr_terminal = (curr_terminal + 1) % MAX_TERMINALS;

    // If the next terminal hasn't been booted yet, boot it on the next PIT interrupt
    if(terminals[curr_terminal].terminal_pcb == NULL)
        return;
    
    set_user_video_page(1);

    pcb_t * next_pcb = terminals[curr_terminal].terminal_pcb;

    // Remap user program page 
    set_user_prog_page(next_pcb->process_id, 1);

    // Update TSS
    tss.esp0 = EIGHT_MB - (next_pcb->process_id * EIGHT_KB) - 4;
    tss.ss0 = KERNEL_DS;

    asm volatile(       //Switch to next process's stack
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "sti"
        : 
        :"r"(next_pcb->curr_esp), "r"(next_pcb->curr_ebp) // Inputs
        :"esp", "ebp"
    );
    
}
