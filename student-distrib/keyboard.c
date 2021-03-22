#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/*
 * init_keyboard
 *    DESCRIPTION: Initializes the keyboard by setting the PIC IRQ
 *    INPUTS/OUTPUTS: none  
 */
void init_keyboard(){

    enable_irq(0x01);

}



