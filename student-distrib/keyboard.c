#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

void init_keyboard(){

    enable_irq(0x01);

}



