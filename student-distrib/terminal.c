
#include "terminal.h"
#include "lib.c"

/*
 * terminal_open
 *    DESCRIPTION: Initalizes all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always 0
 *    SIDE EFFECTS: Initializes all terminal-related vars
 */
int terminal_open() {
    int i;
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        terminal_buf[i] = (char)0;
    }
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
    int i;
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        terminal_buf[i] = (char)0;
    }
    return 0;
}

/*
 * terminal_read
 *    DESCRIPTION: Reads from the buffer 
 *    INPUTS: buf -- ptr to keyboard buffer 
 *    OUTPUTS: copies buf to terminal_buf
 *    RETURN VALUE: Number of bytes written
 *    SIDE EFFECTS: Clears and Overwrites previous buffer
 */
int terminal_read(char * buf) {
    int i;          // Loop index
    char curr;      // Next char in keyboard_buf

    // NULL check input and return 0 if NULL to signify no bytes read
    if(buf == 0)
        return 0; 

    // Clear terminal buffer
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        terminal_buf[i] = 0;
    }

    // Copy keyboard_buf until '\n'
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        curr = buf[i];
        
        // Break from loop if '\n'
        if(curr == '\n')
            break;

        terminal_buf[i] = curr;
    }
    // Return loop index, which is the same as the number of bytes read from buffer not including '\n'
    return i;
}

/*
 * terminal_write
 *    DESCRIPTION: Writes the bytes from input buf to the screen
 *    INPUTS: buf -- bytes to write to screen
 *    OUTPUTS: none
 *    RETURN VALUE: number of bytes/chars written to screen
 *    SIDE EFFECTS: writes to video memory using puts
 */
int terminal_write(char * buf) {
    // NULL check input
    if(buf == 0)
        return -1;
    
    return puts(buf);
}
