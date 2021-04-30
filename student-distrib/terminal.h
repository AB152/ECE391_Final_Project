/* terminal.h - declarations for terminal driver
 *  vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "types.h"

#define MAX_TERMINALS 3

typedef struct{
    struct pcb* terminal_pcb;
    int32_t terminal_id;    //keeps track of which terminal we are on
    int32_t cursor_x;
    int32_t cursor_y;
    int32_t last_assigned_pid;    //keeps track of last assigned pid of the terminal
    volatile int32_t kb_buf_i;    // This terminal's keyboard buffer index
    volatile char kb_enter_flag;
    char kb_buf[KEYBOARD_BUF_SIZE];    // This terminal's keyboard buffer
    uint8_t rtc_freq;                  // RTC frequency for this terminal (virtualization purposes)
}terminal_t;


terminal_t terminals[MAX_TERMINALS];    // Array of terminals to track the 3 running terminals
int32_t scheduled_terminal;  // keeps track of which terminal the scheduler is running
int32_t visible_terminal; // tracks the terminal_id of the currently visible terminal

//------------------------VARS DEPRECATED IN CP5------------------------------

// Terminal's copy of keyboard_buf before it gets cleared (DEPRECATED DUE TO MULTI-TERMINAL)
//char terminal_buf[KEYBOARD_BUF_SIZE]; 

// Terminal's copy of keyboard_buf_i before it gets cleared (DEPRECATED DUE TO MULTI-TERMINAL)
//int32_t terminal_buf_i; 

//--------------------------END DEPRECATED VARS-----------------------------

// Initializes multi-terminal
extern void init_terminal();

// Zeros keyboard buffer, resets enter flag and buffer index
void clear_keyboard_vars(int32_t terminal_id);

// Open syscall for terminal
int32_t terminal_open(int32_t fd);

// Clear terminal specific vars
int32_t terminal_close(int32_t fd);

// Read from keyboard buffer into buf and returns num bytes read
int32_t terminal_read(int32_t fd, void * buf, int32_t n_bytes);

// Writes to the screen from buf and returns num bytes written or -1
int32_t terminal_write(int32_t fd, const void * buf, int32_t n_bytes);

// Switches to the desired terminal
void switch_visible_terminal(int32_t terminal_id);

#endif /* _TERMINAL_H */
