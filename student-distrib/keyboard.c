#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/*
 * init_keyboard
 *    DESCRIPTION: Initializes the keyboard by setting the PIC IRQ and clearing buffer
 *    INPUTS/OUTPUTS: none  
 */
void init_keyboard(){
    int i;      // Loop index
    keyboard_buf_i = 0;
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        keyboard_buf[i] = 0;
    }
    enable_irq(KEYBOARD_IRQ);
}



