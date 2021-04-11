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
int32_t terminal_buf_i;

// Initalizes terminal
int32_t terminal_open(int32_t fd);

// Clear terminal specific vars
int32_t terminal_close(int32_t fd);

// Read from keyboard buffer into buf and returns num bytes read
int32_t terminal_read(int32_t fd, void * buf, int32_t n_bytes);

// Writes to the screen from buf and returns num bytes written or -1
int32_t terminal_write(int32_t fd, const void * buf, int32_t n_bytes);

#endif /* _TERMINAL_H */
