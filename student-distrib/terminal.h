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
    char term_kb_buf[KEYBOARD_BUF_SIZE];    // This terminal's keyboard buffer
}terminal_t;


terminal_t terminals[MAX_TERMINALS];    // Array of terminals to track the 3 running terminals
int32_t curr_terminal;  // keeps track of which terminal the scheduler is running
int32_t visible_terminal; // tracks the terminal_id of the currently visible terminal

// Terminal's copy of keyboard_buf before it gets cleared
char terminal_buf[KEYBOARD_BUF_SIZE];

// Terminal's copy of keyboard_buf_i before it gets cleared
int32_t terminal_buf_i;

extern void init_terminal();

// Initalizes terminal
int32_t terminal_open(int32_t fd);

// Clear terminal specific vars
int32_t terminal_close(int32_t fd);

// Read from keyboard buffer into buf and returns num bytes read
int32_t terminal_read(int32_t fd, void * buf, int32_t n_bytes);

// Writes to the screen from buf and returns num bytes written or -1
int32_t terminal_write(int32_t fd, const void * buf, int32_t n_bytes);

// Switches to the desired terminal
void terminal_switcher(int32_t terminal_id);

#endif /* _TERMINAL_H */
