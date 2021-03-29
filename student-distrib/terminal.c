/* terminal.c - terminal driver functions
 *  vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"

/*
 * terminal_open
 *    DESCRIPTION: Initalizes all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always 0
 *    SIDE EFFECTS: Initializes all terminal-related vars
 */
int terminal_open() {
    return 0;
}

/*
 * terminal_close
 *    DESCRIPTION: Clears all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always 0
 *    SIDE EFFECTS: Clears all terminal-related vars
 */
int terminal_close() {
    return 0;
}

/*
 * terminal_read
 *    DESCRIPTION: Reads from the buffer 
 *    INPUTS: buf -- ptr to output buffer that we copy keyboard_buf to 
 *    OUTPUTS: copies buf to terminal_buf
 *    RETURN VALUE: Number of bytes written
 *    SIDE EFFECTS: Writes to buffer pointed to by input
 */
int terminal_read(char * buf) {
    
    // NULL check input and return 0 if NULL to signify no bytes read
    if(buf == 0)
        return 0; 

    // Block until enter ('\n') has been pressed
    while(!enter_flag);

    // Copy keyboard_buf into input buf (only copy until '\n')
    (void)strncpy((int8_t *)buf, (int8_t *)keyboard_buf, keyboard_buf_i);

    // Return buffer index, which is the same as the number of bytes read from keyboard_buf including '\n'
    return keyboard_buf_i;
}

/*
 * terminal_write
 *    DESCRIPTION: Writes the bytes from input buf to the screen
 *    INPUTS: buf -- bytes to write to screen
 *    OUTPUTS: none
 *    RETURN VALUE: number of bytes/chars written to screen
 *    SIDE EFFECTS: writes to video memory using putc
 */
int terminal_write(char * buf, int n_bytes) {
    int i;      // Loop index

    // NULL check input
    if(buf == 0 || n_bytes < 0)
        return -1;

    // Print n_bytes worth of chars
    for(i = 0; i < n_bytes; i++) {
        putc(buf[i]);
    }
    return i;
}
