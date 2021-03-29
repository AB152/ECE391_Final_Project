/* terminal.h - declarations for terminal driver
 *  vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"

// Initalizes terminal
int terminal_open();

// Clear terminal specific vars
int terminal_close();

// Read from keyboard buffer into buf and returns num bytes read
int terminal_read(char * buf);

// Writes to the screen from buf and returns num bytes written or -1
int terminal_write(char * buf, int n_bytes);

#endif /* _TERMINAL_H */
