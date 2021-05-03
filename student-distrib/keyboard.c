/* keyboard.c - implementation for keyboard
 *  vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"
#include "asm_linkage.h"
#include "x86_desc.h"
#include "terminal.h"
#include "i8259.h"

/*

 * init_keyboard
 *    DESCRIPTION: Initializes the keyboard by setting the IDT entry, PIC IRQ, and clearing buffer
 *    INPUTS/OUTPUTS: none  
 */
void init_keyboard(){
    clear_keyboard_buf();
    enable_irq(KEYBOARD_IRQ);
    enter_flag = 0;
    SET_IDT_ENTRY(idt[0x21], &keyboard_processor);        //index 21 of IDT reserved for keyboard
}

/*
 * clear_keyboard_buf
 *    DESCRIPTION: Clears keyboard buffer
 *    INPUTS/OUTPUTS: none  
 */
void clear_keyboard_buf() {
    int i;      // Loop index
    keyboard_buf_bytes_written = 0;
    keyboard_cursor_pos = 0;
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        keyboard_buf[i] = 0;
    }
}

/*
 * keyboard_handler
 *    DESCRIPTION: Handler for keyboard interrupts
 *    INPUTS/OUTPUTS: none  
 */
void keyboard_handler() {
    /* scan_code stores the hex value of the key that is stored in the keyboard port */
    // Scan code + 0x80 is that key but released/"de-pressed"
    // ASCII + 0x20 is the lower case of that letter
    
    // Index is the scan code, the value at an index is that scan code key's ASCII
    int scan_code_to_ascii[] = {
    0, 0x1B, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', 0, 0, 0, 0, 0
    };

    int temp_i;   // Temp var used for any misc loop index

    // Get scan code from keyboard
    int scan_code = inb(KEYBOARD_PORT);

    // Set flags for when ASCII modifying keys are pressed
    switch(scan_code){
        case LEFT_SHIFT_PRESSED:
            left_shift_flag = 1;
            break;
        case LEFT_SHIFT_RELEASED:
            left_shift_flag = 0;
            break;
        case RIGHT_SHIFT_PRESSED:
            right_shift_flag = 1;
            break;
        case RIGHT_SHIFT_RELEASED:
            right_shift_flag = 0;
            break;
        case CAPS_LOCK_PRESSED:
            caps_flag = !caps_flag;
            break;
        case LEFT_CTRL_PRESSED:
            ctrl_flag = 1;
            break;
        case LEFT_CTRL_RELEASED:
            ctrl_flag = 0;
            break;
        case LEFT_ALT_PRESSED:
            alt_flag = 1;
            break;
        case LEFT_ALT_RELEASED:
            alt_flag = 0;
            break;
    }

    // EC: Note that keys 2, 4, 6, and 8 on keypad mimick the arrow keys as they use the same scan codes
    // EC: Up arrow goes down in command_history stack, copies into kb_buf, and prints to screen
    if(scan_code == UP_ARROW) {
        command_history_up_arrow();
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // EC: Down arrow goes up in command_history stack, copies into kb_buf, and prints to screen
    if(scan_code == DOWN_ARROW) {
        command_history_down_arrow();
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // EC: Left arrow moves stdin stream cursor to the left
    if(scan_code == LEFT_ARROW) {
        // Only move cursor to the left if it already isn't at the start of the stream
        if(keyboard_cursor_pos > 0) {
            keyboard_cursor_pos--;
            shift_cursor(-1);
        }
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // EC: Right arrow moves stdin stream cursor to the right
    if(scan_code == RIGHT_ARROW) {
        // Only move cursor to the right if the cursor isn't already at the end of the stream
        if(keyboard_cursor_pos < keyboard_buf_bytes_written) {
            keyboard_cursor_pos++;
            shift_cursor(1);
        }
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // Ignore key releases (F1 pressed is 0x3B, any scan codes greater than that are releases)
    if(scan_code >= 0x3B || scan_code == LEFT_SHIFT_PRESSED || scan_code == RIGHT_SHIFT_PRESSED || scan_code == CAPS_LOCK_PRESSED ||
        scan_code == LEFT_CTRL_PRESSED || scan_code == LEFT_ALT_PRESSED) {
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // Convert scan code to ASCII equivalent
    char key_pressed = scan_code_to_ascii[scan_code];

    // Backspace pressed: delete last char if buffer isn't empty, then return from interrupt
    if(key_pressed == '\b') {
        if(keyboard_buf_bytes_written > 0) {
            keyboard_cursor_pos--;
            keyboard_buf[keyboard_cursor_pos] = 0;
            putc('\b');
        }
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    
    // If enter pressed, print newline, set enter_flag for terminal, mark buf for clear, and return from interrupt
    if(key_pressed == '\n') {
        keyboard_buf[keyboard_buf_bytes_written] = key_pressed;
        keyboard_buf_bytes_written++;
        enter_flag = 1;
        putc('\n');
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // Ctrl + l and Ctrl + L clears screen and prints keyboard buffer again
    if(ctrl_flag && (key_pressed == 'l' || key_pressed == 'L')) {
        clear();
        terminal_write(NULL, keyboard_buf, keyboard_buf_bytes_written);
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    if(alt_flag) {
        // Case for alt flag
    }
    
    // Don't print non-printing keys
    if(key_pressed == 0) {
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // If entering a char will overflow either buffer (only buf_size-1 chars + '\n' allowed), ignore the key press
    if(keyboard_buf_bytes_written == KEYBOARD_BUF_CHAR_MAX || keyboard_buf_bytes_written == terminal_buf_n_bytes - 1) {
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    

    /* IMPORTANT */
    // Ask TA if tab with less than 8 spaces left in buffer continues tab on next line
    // Also ask TA if we need to process multiple letters pressed at same time
    if(key_pressed == '\t') {
        // Tab = 8 spaces, but clip if overflow
        for(temp_i = 0; temp_i < 8; temp_i++) {
            if(keyboard_buf_bytes_written < KEYBOARD_BUF_CHAR_MAX && keyboard_buf_bytes_written < terminal_buf_n_bytes - 1) {
                keyboard_buf[keyboard_buf_bytes_written] = ' ';
                keyboard_buf_bytes_written++;
                putc(' ');
            }
            else 
                break;
        }
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // Handle caps lock and shift for letter keys (ignored if shift is held by XOR)
    if((caps_flag ^ (left_shift_flag || right_shift_flag)) && (key_pressed >= 97 && key_pressed <= 122)){
        key_pressed = key_pressed - 32; // Subtracting letter ASCII by 32 maps to caps
    }

    // Handle shift for special characters
    if((left_shift_flag || right_shift_flag)) {
        // Map special characters
        switch(key_pressed){
            case '`':
                key_pressed = '~';
                break;
            case '1':
                key_pressed = '!';
                break;
            case '2':
                key_pressed = '@';
                break;
            case '3':
                key_pressed = '#';
                break;
            case '4':
                key_pressed = '$';
                break;
            case '5':
                key_pressed = '%';
                break;
            case '6':
                key_pressed = '^';
                break;
            case '7':
                key_pressed = '&';
                break;
            case '8':
                key_pressed = '*';
                break;
            case '9':
                key_pressed = '(';
                break;
            case '0':
                key_pressed = ')';
                break;
            case '-':
                key_pressed = '_';
                break;
            case '=':
                key_pressed = '+';
                break;
            case '[':
                key_pressed = '{';
                break;
            case ']':
                key_pressed = '}';
                break;
            case '\\':
                key_pressed = '|';
                break;
            case ';':
                key_pressed = ':';
                break;
            case '\'':
                key_pressed = '"';
                break;
            case ',':
                key_pressed = '<';
                break;
            case '.':
                key_pressed = '>';
                break;
            case '/':
                key_pressed = '?';
                break;
        }
    }

    // Put key pressed in buffer and on screen and advance buffer index
    keyboard_buf[keyboard_buf_bytes_written] = key_pressed;
    keyboard_buf_bytes_written++;        
    putc(key_pressed);
    
    // Send EOI to PIC
    send_eoi(KEYBOARD_IRQ);         // 0x01 is IRQ number for keyboard
}

