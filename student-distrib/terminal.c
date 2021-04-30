/* terminal.c - terminal driver functions
 *  vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "paging.h"
#include "system_calls.h"


/*
 * init_terminal
 *    DESCRIPTION: Initalizes all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: none
 *    SIDE EFFECTS: Initializes all terminal-related vars
 */
void init_terminal(){
    // Find next available terminal ID to assign
    int i, j; 
    curr_terminal = 0;
    visible_terminal = 0;
    for(i = 0; i < MAX_TERMINALS; i++) {    
        terminals[i].terminal_pcb = NULL;   //will be implemented in scheduler
        terminals[i].terminal_id=i;
        terminals[i].cursor_x = 0;
        terminals[i].cursor_y = 0;
        terminals[i].last_assigned_pid=-1;        //flag as no process running
        terminals[i].term_kb_enter_flag = 0;
        terminals[i].term_kb_buf_i = 0;
        for(j=0; j<KEYBOARD_BUF_SIZE; j++){
            terminals[i].term_kb_buf[j]='\0';
        }
    }
}

/*
 * terminal_open
 *    DESCRIPTION: Open the terminal (stdin)
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always 0 (success)
 *    SIDE EFFECTS: none
 */
int32_t terminal_open(int32_t fd) {
    return 0;
}

/*
 * terminal_close
 *    DESCRIPTION: This should never be called
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always -1 as terminal should never be closed
 *    SIDE EFFECTS: none
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

    // Let keyboard know how many bytes the buffer is (is this effectively useless?)
    terminal_buf_n_bytes = n_bytes;

    // Block until enter ('\n') has been pressed for the scheduled terminal
    while(!terminals[curr_terminal].term_kb_enter_flag);

    // Alias var for readability (thanks to the if-case at line 92)
    char * terminal_kb_buf = terminals[curr_terminal].term_kb_buf;

    // Copy keyboard_buf for parsing
    (void)strncpy((int8_t *)terminal_kb_buf, (int8_t *)keyboard_buf, n_bytes);

    // Copy count of chars written from keyboard
    terminals[curr_terminal].term_kb_buf_i = keyboard_buf_i;

    // Strip any extraneous chars after '\n' due to race cond in issue #18's reopen
    if(terminals[curr_terminal].term_kb_buf_i > 0) { 
        if(terminal_kb_buf[terminals[curr_terminal].term_kb_buf_i - 1] != '\n') {
            terminal_kb_buf[terminals[curr_terminal].term_kb_buf_i - 1] = '\0';
            terminals[curr_terminal].term_kb_buf_i--;
        }
    }

    // Copy terminal_buf to input buf
    (void)strncpy((int8_t *)buf, (int8_t *)terminal_kb_buf, n_bytes);

    // Reset so the keyboard buffer can write to its normal size
    terminal_buf_n_bytes = KEYBOARD_BUF_SIZE;
    terminals[curr_terminal].term_kb_enter_flag = 0;
    clear_keyboard_buf();

    // Return keyboard_buf index, which is the same as the number of bytes read from keyboard_buf including '\n'
    return terminals[curr_terminal].term_kb_buf_i;
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

    return i;
}

/*
 * switch_visible_terminal
 *    DESCRIPTION: Switches to the desired terminal
 *    INPUTS: terminal_id -- the terminal to switch to
 *    OUTPUTS: none
 *    RETURN VALUE: none
 *    SIDE EFFECTS: Switches to the desired terminal (including video page)
 */
void switch_visible_terminal(int32_t terminal_id) {
    if(terminal_id==visible_terminal)  //check if we are switching to same terminal
        return;

    // Set video memory page
    change_terminal_video_page(visible_terminal, terminal_id);

    // Preserve keyboard buffer and coordinates for screen and cursor
    memcpy(&(terminals[visible_terminal].term_kb_buf), keyboard_buf, KEYBOARD_BUF_SIZE);
    terminals[visible_terminal].term_kb_buf_i = keyboard_buf_i;
    terminals[visible_terminal].cursor_x = get_screen_x();
    terminals[visible_terminal].cursor_y = get_screen_y();
    
    // Update keyboard buffer and screen coordinates
    memcpy(keyboard_buf, &(terminals[terminal_id].term_kb_buf), KEYBOARD_BUF_SIZE);
    keyboard_buf_i = terminals[terminal_id].term_kb_buf_i;
    update_cursor(terminals[terminal_id].cursor_x, terminals[terminal_id].cursor_y);

    visible_terminal = terminal_id;  //update visible terminal id to the one we switch to
}
