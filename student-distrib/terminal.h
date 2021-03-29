/* terminal.h - declarations for terminal driver
 *  vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "types.h"

// Terminal's copy of keyboard_buf before it gets cleared
char terminal_buf[KEYBOARD_BUF_SIZE];

// Terminal's copy of keyboard_buf_i before it gets cleared
int terminal_buf_i;

// Initalizes terminal
int terminal_open();

// Clear terminal specific vars
int terminal_close(int32_t fd);

// Read from keyboard buffer into buf and returns num bytes read
int terminal_read(int32_t fd, char * buf, int32_t n_bytes);

// Writes to the screen from buf and returns num bytes written or -1
int terminal_write(int32_t fd, char * buf, int32_t n_bytes);

#endif /* _TERMINAL_H */
