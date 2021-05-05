/* terminal.h - declarations for terminal driver
 *  vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "types.h"

#define COMMAND_HISTORY_SIZE 20

// Terminal's copy of keyboard_buf before it gets cleared
char terminal_buf[KEYBOARD_BUF_SIZE];

// EC: Command history (stack-like)
uint8_t command_history_stack[COMMAND_HISTORY_SIZE][KEYBOARD_BUF_SIZE];

// EC: Command history index (kinda like a stack pointer)
int32_t command_history_stack_pointer;

// Terminal's copy of keyboard_buf_i before it gets cleared
int32_t terminal_buf_i;

// Initalizes terminal
int32_t terminal_open(int32_t fd);

// Clear terminal specific vars
int32_t terminal_close(int32_t fd);

// Read from keyboard buffer into buf and returns num bytes read
int32_t terminal_read(int32_t fd, void * buf, int32_t n_bytes);

// Writes to the screen from buf and returns num bytes written or -1
int32_t terminal_write(int32_t fd, const void * buf, int32_t n_bytes);

// EC: Clear command history stack
void clear_command_history();

// EC: Helper function to push to command_history stack
void push_to_command_history(const uint8_t* command);

// EC: Handler for when the up arrow is pressed
void command_history_up_arrow();

// EC: Handler for when the down arrow is pressed
void command_history_down_arrow();

// EC: Prints the bootup splash screen
void bootup_splash_screen();

#endif /* _TERMINAL_H */
