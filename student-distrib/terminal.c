/* terminal.c - terminal driver functions
 *  vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"

int allow_arrow_keys = 0;

// If the user types something but then up arrows before hitting enter, save it in this
uint8_t keyboard_save[KEYBOARD_BUF_SIZE]; 
int keyboard_save_i = 0;

/*
 * terminal_open
 *    DESCRIPTION: Initalizes all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always 0
 *    SIDE EFFECTS: Initializes all terminal-related vars
 */
int32_t terminal_open(int32_t fd) {
    int i;
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

    allow_arrow_keys = 1;

    // Block until enter ('\n') has been pressed
    while(!enter_flag);

    allow_arrow_keys = 0;

    // Copy keyboard_buf into terminal buf
    (void)strncpy((int8_t *)terminal_buf, (int8_t *)keyboard_buf, n_bytes);

    // Copy count of chars written from keyboard
    terminal_buf_i = keyboard_buf_bytes_written;

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

    // Reset command history state
    command_history_stack_pointer = 0;
    memset(keyboard_save, '\0', KEYBOARD_BUF_SIZE);

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
        terminal_write(NULL, keyboard_buf, keyboard_buf_bytes_written);
    }

    return i;
}

/*
* IMPORTANT!!! shell mixes with other command when going through terminal history
* Might be bc of nested shell
*/


/*
 * clear_command_history (EC)
 *    DESCRIPTION: Re-initializes command history related vars
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: none
 *    SIDE EFFECTS: Uses memset to clear the 2D array and keyboard_save
 */
void clear_command_history() {
    int i;
    command_history_stack_pointer = 0;
    keyboard_save_i = 0;
    memset(keyboard_save, '\0', KEYBOARD_BUF_SIZE);
    for(i = 0; i < COMMAND_HISTORY_SIZE; i++) {
        memset(command_history_stack[i], '\0', KEYBOARD_BUF_SIZE);
    }
}

/*
 * push_to_command_history (EC)
 *    DESCRIPTION: Pushes input to command history stack
 *    INPUTS: command -- string to add to history stack
 *    OUTPUTS: none
 *    RETURN VALUE: none
 *    SIDE EFFECTS: If stack is full, the bottom most entry gets overwritten by the one above it
 *    NOTES: We don't need to strip newlines as the shell already does that before it calls execute. 
 */
void push_to_command_history(const uint8_t* command) {
    int i;
    // Add to stack by moving existing entries down (redundancies possible)
    for(i = COMMAND_HISTORY_SIZE - 1; i > 0; i--) {
        memcpy(command_history_stack[i], command_history_stack[i - 1], KEYBOARD_BUF_SIZE);
    }
    // Push command to top of stack using hard copy method
    uint32_t length = strlen((int8_t *)command);
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        if(i < length)
            command_history_stack[0][i] = command[i];
        else
            command_history_stack[0][i] = '\0';
    }
}

/*
 * command_history_up_arrow (EC)
 *    DESCRIPTION: Handler for when the up arrow is pressed on the keyboard
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: none
 *    SIDE EFFECTS: TODO
 *    NOTES: Should do nothing unless we're in terminal_read
 */
void command_history_up_arrow() {
    // Run only if we're in terminal_read, if we aren't out of bounds, and the next item in stack actually exists
    if(!allow_arrow_keys || command_history_stack_pointer == COMMAND_HISTORY_SIZE || command_history_stack[command_history_stack_pointer][0] == '\0')
        return;

    // Save keyboard_buf if we're at the top of the stack (net stack displacement is zero)
    if(command_history_stack_pointer == 0) {
        memcpy(keyboard_save, keyboard_buf, KEYBOARD_BUF_SIZE);
        keyboard_save_i = keyboard_buf_bytes_written;
    }
    
    int i;  // Loop index to clear stdin stream in terminal video mem
    
    // Clear stdin stream in video memory
    for(i = keyboard_buf_bytes_written; i > 0; i--) {
        putc('\b');
    }
    
    // Copy over item currently pointed to in stack into keyboard buffer
    uint8_t * next_item = command_history_stack[command_history_stack_pointer];
    memcpy(keyboard_buf, next_item, KEYBOARD_BUF_SIZE);
    keyboard_buf_bytes_written = (int)strlen((int8_t *)next_item);
    keyboard_cursor_pos = keyboard_buf_bytes_written;

    // Write the history's command to the screen by redrawing the new keyboard_buf (could use printf but oh well)
    for(i = 0; i < keyboard_buf_bytes_written; i++) {
        putc((uint8_t)keyboard_buf[i]);
    }

    // Increment stack pointer
    command_history_stack_pointer++;
}

/*
 * command_history_down_arrow (EC)
 *    DESCRIPTION: Handler for when the down arrow is pressed on the keyboard
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: none
 *    SIDE EFFECTS: TODO
 *    NOTES: Should do nothing unless we're in terminal_read
 */
void command_history_down_arrow() {
    // Run only if we're in terminal_read
    if(!allow_arrow_keys)
        return;

    int i;  // Loop index to clear stdin stream in terminal video mem

    // See if we're at the top of the stack (net stack displacement of zero)
    if(command_history_stack_pointer == 0) {
        // Do nothing if there is nothing saved
        if(keyboard_save[0] == '\0')
            return;
        // Restore from saver 
        memcpy(keyboard_buf, keyboard_save, KEYBOARD_BUF_SIZE);
        // Clear stdin stream in video memory
        for(i = keyboard_buf_bytes_written; i > 0; i--) {
            putc('\b');
        }
        // Write the saved buffer to the screen
        terminal_write(NULL, keyboard_buf, keyboard_buf_bytes_written);
        return;
    }

    // Clear stdin stream in video memory
    for(i = keyboard_buf_bytes_written; i > 0; i--) {
        putc('\b');
    }

    // Decrement stack pointer
    command_history_stack_pointer--;

    // Copy over item currently pointed to in stack to keyboard buffer
    uint8_t * next_item = command_history_stack[command_history_stack_pointer];
    memcpy(keyboard_buf, next_item, KEYBOARD_BUF_SIZE);
    keyboard_buf_bytes_written = (int)strlen((int8_t *)next_item); 
    keyboard_cursor_pos = keyboard_buf_bytes_written;

    // Write the history's command to the screen by redrawing the new keyboard_buf (could use printf but oh well)
    for(i = 0; i < keyboard_buf_bytes_written; i++) {
        putc((uint8_t)keyboard_buf[i]);
    }
}
