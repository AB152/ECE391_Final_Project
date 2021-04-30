#include "scheduler.h"
#include "system_calls.h"
#include "terminal.h"
#include "i8259.h"
#include "pit.h"
#include "paging.h"
#include "x86_desc.h"
#include "rtc.h"


int shell_count = 0;

/*
 * scheduler
 *    DESCRIPTION: Performs process switching
 *    INPUTS: none
 *    OUTPUTS: none
 *    SIDE EFFECTS: Switches active process using round-robin scheduling
 */
void scheduler(){   
    // Get the current active process's PCB
    pcb_t * curr_pcb = terminals[scheduled_terminal].terminal_pcb;

    if(shell_count == 0){
        init_RTC();
        RTC_open(NULL);
    }
        
    // Boot up the three terminals
    if(curr_pcb == NULL && shell_count < 3){
        pcb_t temp_pcb;
        asm volatile(       // Retrieve initial ESP and EBP before booting base shells
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r"(temp_pcb.curr_esp), "=r"(temp_pcb.curr_ebp) // Outputs
        );
        shell_count++;
        terminals[scheduled_terminal].terminal_pcb = &temp_pcb;
        terminals[scheduled_terminal].last_assigned_pid = scheduled_terminal;   //mark terminal as booted and initialize its pid
        //terminals[scheduled_terminal].terminal_pcb->parent_pcb = (pcb_t *)(EIGHT_MB - ((count + 1) * EIGHT_KB));

        terminals[scheduled_terminal].rtc_freq = 2; // Set RTC Frequency to 2
        RTC_write(NULL, 2, NULL); // Set RTC Frequency to 2

        switch_visible_terminal(scheduled_terminal);    // Switch video page so the bootup text stays in that terminal
        printf("Terminal %d booting...\n", shell_count);
        execute((uint8_t *)"shell");
    }

    // Save old process's stack (also properly sets up first base shell's ESP/EBP)
    asm volatile(       
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(curr_pcb->curr_esp), "=r"(curr_pcb->curr_ebp) // Outputs
    );

    // Round-robin increment
    scheduled_terminal = (scheduled_terminal + 1) % MAX_TERMINALS;

    // If the next terminal hasn't been booted yet, boot it on the next PIT interrupt
    if(terminals[scheduled_terminal].terminal_pcb == NULL)
        return;
    
    set_user_video_page(1);

    pcb_t * next_pcb = terminals[scheduled_terminal].terminal_pcb;

    curr_freq = terminals[scheduled_terminal].rtc_freq;
    
    RTC_write(NULL, curr_freq, NULL); // Set RTC Frequency to the terminals RTC Freq

    // Remap user program page 
    set_user_prog_page(next_pcb->process_id, 1);

    // Update TSS
    tss.esp0 = EIGHT_MB - (next_pcb->process_id * EIGHT_KB) - 4;
    tss.ss0 = KERNEL_DS;

    // IMPORTANT: Why tf do we need an STI here? Should we ask a TA?
    asm volatile(       //Switch to next process's stack
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "sti"
        : 
        :"r"(next_pcb->curr_esp), "r"(next_pcb->curr_ebp) // Inputs
        :"esp", "ebp"
    );
    
}
