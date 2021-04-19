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
int32_t terminal_open(int32_t fd) {
    int i = 0;
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        terminal_buf[i] = 0;
    }
    return 0;
}

/*
 * terminal_close
 *    DESCRIPTION: Clears all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always -1 as it should never be closed
 *    SIDE EFFECTS: Clears all terminal-related vars
 */
int32_t terminal_close(int32_t fd) {
    return -1;
}

/*
 * terminal_read
 *    DESCRIPTION: Reads from the buffer 
 *    INPUTS: file descriptor, buf -- ptr to output buffer that we copy keyboard_buf to, n_bytes
 *    OUTPUTS: copies buf to terminal_buf
 *    RETURN VALUE: Number of bytes written
 *    SIDE EFFECTS: Writes to buffer pointed to by input
 */
int32_t terminal_read(int32_t fd, void * buf, int32_t n_bytes) {
    
    // NULL check input and return 0 if NULL to signify no bytes read
    if(buf == 0 || n_bytes <= 0)
        return 0; 

    // Let keyboard know how many bytes the buffer is
    terminal_buf_n_bytes = n_bytes;

    // Block until enter ('\n') has been pressed
    while(!enter_flag);

    // Copy keyboard_buf into terminal buf
    (void)strncpy((int8_t *)terminal_buf, (int8_t *)keyboard_buf, n_bytes);

    // Copy count of chars written from keyboard
    terminal_buf_i = keyboard_buf_i;

    // Strip any extraneous chars after '\n' due to race cond in issue #18's reopen
    if(terminal_buf_i > 0) { 
        if(terminal_buf[terminal_buf_i - 1] != '\n') {
            terminal_buf[terminal_buf_i - 1] = '\0';
            terminal_buf_i--;
        }
    }

    // Copy terminal_buf to input buf
    (void)strncpy((int8_t *)buf, (int8_t *)terminal_buf, n_bytes);

    // Reset so the keyboard buffer can write to its normal size
    terminal_buf_n_bytes = KEYBOARD_BUF_SIZE;
    enter_flag = 0;
    clear_keyboard_buf();

    // Return keyboard_buf index, which is the same as the number of bytes read from keyboard_buf including '\n'
    return terminal_buf_i;
}

/*
 * terminal_write
 *    DESCRIPTION: Writes the bytes from input buf to the screen
 *    INPUTS: buf -- bytes to write to screen
 *    OUTPUTS: none
 *    RETURN VALUE: number of bytes/chars written to screen
 *    SIDE EFFECTS: writes to video memory using putc
 */
int32_t terminal_write(int32_t fd, const void * buf, int32_t n_bytes) {

    int i;      // Loop index

    // NULL check input
    if(buf == 0 || n_bytes < 0)
        return -1;

    // Print n_bytes worth of chars
    for(i = 0; i < n_bytes; i++) {
        putc(((char *)(buf))[i]);
    }

    /* Stops the shell prompt from being backspaced if we type before the prompt is printed (like during fish)
       Prints out the keyboard buffer again if the shell prompt (length 7) is the argument */
    if(!strncmp((int8_t *)buf, "391OS> ", 7)) {
        terminal_write(NULL, keyboard_buf, keyboard_buf_i);
    }

    return i;
}
