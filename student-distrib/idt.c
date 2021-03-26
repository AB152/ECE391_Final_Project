/* idt.c (MP3.1) - All IDT related functions
 * vim:ts=4 noexpandtab
 */

#include "idt.h"

#include "idt_ex_handler.h"

#include "keyboard.h"

#include "rtc.h"

#include "i8259.h"

#include "lib.h"



/*
* enter all relevant exceptions into IDT table
*/


// Refer to OSDEV for paging: flush, map page directory to memory correctly

/*
 * init_IDT
 *    DESCRIPTION: Initialize IDT but does not set the handler offset fields for each vector
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: none
 *    SIDE EFFECTS: Fills entries of IDT as interrupt gates but without setting offset fields
 *    NOTES: See OSDev and Intel manual for how to set up interrupt gates in the IDT
 *           The offset fields should be initialized using macro SET_IDT_ENTRY(str, handler)
 *               upon creating those functions
 */ 
void init_IDT(){
    int i;                                      // Loop index
    for(i = 0; i < NUM_VEC; i++) {
        
        // Advance to vector 0x20 as we won't fill in vectors 0x14 thru 0x1F (see 3.1 hints)
        if(i == 0x14)
            i = 0x20;
        // Skip vector 15 as it's reserved by Intel (tests.c says use it for assertions?)
        else if(i == 15)
            continue;

        idt_desc_t next_entry;                  // Next idt entry that we are initializing
        next_entry.seg_selector = KERNEL_CS;    // initialize to kernel's code segment descriptor
        next_entry.reserved4 = 0x0;             // interrupt gate reserved bits are 0 0 1 1 0 for 32 bit size
        next_entry.reserved3 = 0x0;
        next_entry.reserved2 = 0x1;
        next_entry.reserved1 = 0x1;
        next_entry.size = 0x1;                  // size always 32-bits
        next_entry.reserved0 = 0x0;     
        next_entry.dpl = 0x0;                   // set privilege level
        next_entry.present = 0x1;               // set present to 1 to show that this interrupt is active
        
        // The offset fields should be initialized using macro SET_IDT_ENTRY(str, handler), not here

        // If we're filling in system call vector (0x80), change DPL to user ring
        if(i == 0x80)
            next_entry.dpl = 0x03;              // System calls should always have user level DPL (ring 3)

        // What do we do for val[2] in the union?

        // Populate IDT vector with new entry
        idt[i] = next_entry;    }

    /*
    * enter in exceptions and devices into the appropriate indices of the IDT table
    * IMPORTANT: Piazza (@882) said that we "shouldn't hard code DEVICE handlers into the IDT"
    */
    SET_IDT_ENTRY(idt[0], &divide_by_zero);             //exception 0
    SET_IDT_ENTRY(idt[1], &debug);                      //exception 1
    SET_IDT_ENTRY(idt[2], &nm_interrupt);               //exception 2
    SET_IDT_ENTRY(idt[3], &breakpoint);                 //exception 3
    SET_IDT_ENTRY(idt[4], &overflow);                   //exception 4
    SET_IDT_ENTRY(idt[5], &br_exceeded);                //exception 5
    SET_IDT_ENTRY(idt[6], &inv_opcode);                 //exception 6
    SET_IDT_ENTRY(idt[7], &device_na);                  //exception 7
    SET_IDT_ENTRY(idt[8], &double_fault);               //exception 8
    SET_IDT_ENTRY(idt[9], &cp_seg_overrun);             //exception 9
    SET_IDT_ENTRY(idt[10], &inv_tss);                   //exception 10
    SET_IDT_ENTRY(idt[11], &seg_not_present);           //exception 11
    SET_IDT_ENTRY(idt[12], &stack_fault);               //exception 12
    SET_IDT_ENTRY(idt[13], &gen_protection);            //exception 13
    SET_IDT_ENTRY(idt[14], &page_fault);                //exception 14
    SET_IDT_ENTRY(idt[16], &fpu_floating_point);        //exception 16 (exception 15 is reserved by Intel)
    SET_IDT_ENTRY(idt[17], &alignment_check);           //exception 17
    SET_IDT_ENTRY(idt[18], &machine_check);             //exception 18
    SET_IDT_ENTRY(idt[19], &simd_floating_point);       //exception 19

    SET_IDT_ENTRY(idt[0x21], &keyboard_processor);        //index 21 of IDT reserved for keyboard
    SET_IDT_ENTRY(idt[0x28], &RTC_processor);             //index 28 of IDT reserved for RTC


}

// Handles interrupt (print error message and other relevant items like regs)
// Is stack trace required?
void exception_handler(int32_t interrupt_vector){
    //clear();
    switch(interrupt_vector){
        case 0xFFFFFFFF:
            printf(" Divide By Zero Exception");             //print all resepctive exceptions
            while(1);                                        //wait indefinitely
        case 0xFFFFFFFE:
            printf(" Debug Exception");
            while(1);
        case 0xFFFFFFFD:
            printf(" Non-masking Interrupt Exception");
            while(1);
        case 0xFFFFFFFC:
            printf(" Breakpoint Exception");
            while(1);
        case 0xFFFFFFFB:
            printf(" Overflow Exception");
            while(1);
        case 0xFFFFFFFA:
            printf(" Bound Range Exception");
            while(1);
        case 0xFFFFFFF9:
            printf(" Invalid Opcode Exception");
            while(1);
        case 0xFFFFFFF8:
            printf(" Device Not Available");
            while(1);
        case 0xFFFFFFF7:
            printf(" Double Fault Exception");
            while(1);
        case 0xFFFFFFF6:
            printf(" Coprocessor Segment Overrun");
            while(1);
        case 0xFFFFFFF5:
            printf(" Invalid TSS Exception");
            while(1);
        case 0xFFFFFFF4:
            printf(" Segment Not Present");
            while(1);
        case 0xFFFFFFF3:
            printf(" Stack Fault Exception");
            while(1);
        case 0xFFFFFFF2:
            printf(" General Protection Exception");
            while(1);
        case 0xFFFFFFF1:
            printf(" Page-Fault Exception");
            while(1);
        case 0xFFFFFFEF:
            printf(" x87 FPU Floating-Point Error");
            while(1);
        case 0xFFFFFFEE:
            printf(" Alignment Check Exception");
            while(1);
        case 0xFFFFFFED:
            printf(" Machine-Check Exception");
            while(1);
        case 0xFFFFFFEC:
            printf(" SIMD Floating-Point Exception");
            while(1);
    }
}


void keyboard_handler() {
    /* scan_code stores the hex value of the key that is stored in the keyboard port */
    // Scan code + 0x80 is that key but released/"de-pressed"
    // ASCII + 0x20 is the lower case of that letter
    
    // int scan_code_to_ascii[] = {
    // 0, 26, 49, 50, 51, 52, 53, 54, 55, 56,
    // 57, 48, 45, 61, 8, 9, 113, 119, 101, 114,
    // 116, 121, 117, 105, 111, 112, 91, 93, 10, 0,
    // 97, 115, 100, 102, 103, 104, 106, 107, 108, 59,
    // 39, 96, 0, 92, 122, 120, 99, 118, 98, 110,
    // 109, 44, 46, 47, 0, 42, 0, 32, 0, 0
    // 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0, 55, 56, 57, 45, 52, 53, 54, 43, 49,
    // 50, 51, 48, 46, 0, 0, 0, 0, 0
    // };
    
    // Index is the scan code, the value at an index is that scan code key's ASCII
    int scan_code_to_ascii[] = {
    0, 0x1A, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', 0x08, 0x09, 'q', 'w', 'e', 'r',
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

    // Set flags is control character key is pressed
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
        case TAB_PRESSED:
            tab_flag = 1;
            break;
        case TAB_RELEASED:
            tab_flag = 0;
            break;
        case ENTER_PRESSED:
            enter_flag = 1;
            break;
        case ENTER_RELEASED:
            enter_flag = 0;
            break;
        case BACKSPACE_PRESSED:
            backspace_flag = 1;
            break;
        case BACKSPACE_RELEASED:
            backspace_flag = 0;
            break;
    }

    // Delete last char if backspace pressed and buffer isn't empty, then return from interrupt
    if(backspace_flag && keyboard_buf_i > 0) {
        keyboard_buf_i--;
        keyboard_buf[keyboard_buf_i] = 0;
        // IMPORTANT: figure out how to delete last char from screen
        send_eoi(KEYBOARD_IRQ);
        return;
    }

    // If entering a char will overflow buffer, ignore the key press and end interrupt
    // Also ignore key releases (F12 pressed is 0x58, any scan codes greater than that are unneeded)
    if(keyboard_buf_i == KEYBOARD_BUF_SIZE || scan_code > 0x58) {
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    
    // Convert scan code to ASCII equivalent
    char key_pressed = scan_code_to_ascii[scan_code];

    if((caps_flag ^ (left_shift_flag || right_shift_flag)) && (key_pressed >= 97 && key_pressed >= 122)){
        key_pressed = key_pressed - 32; // Map to caps
    }

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

    if(ctrl_flag && (key_pressed == 'l' || key_pressed == 'L')) {
        // Clear buffer and put cursor to the left
    }

    if(alt_flag) {
        // Case for alt flag
    }
    
    if(tab_flag) {
        // Tab = 8 spaces, but clip if overflow
        for(temp_i = 0; temp_i < 8; temp_i++) {
            if(keyboard_buf_i < KEYBOARD_BUF_SIZE) {
                keyboard_buf[keyboard_buf_i] = ' ';
                keyboard_buf_i++;
                putc(' ');
            }
            else 
                break;
        }
        /* IMPORTANT */
        // Ask TA if tab with less than 8 spaces left in buffer continues tab on next line
        // Also ask TA if we need to process multiple letters pressed at same time
    }
    
    // enter_flag might be unnecessary and we can just use ASCII '\n'
    if(enter_flag) {
        // Scroll up if screen is full
        putc('\n');
    }

    // Put key pressed in buffer and on screen and advance buffer index
    keyboard_buf[keyboard_buf_i] = key_pressed;
    keyboard_buf_i++;        
    putc(key_pressed);
    
    // Send EOI to PIC
    send_eoi(KEYBOARD_IRQ);         // 0x01 is IRQ number for keyboard
}

/*
 * RTC_interrupt
 *    DESCRIPTION: RTC register C needs to be read, so interupts will happen again
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: none
 *    SIDE EFFECTS: set RTC_int to 1
 *    NOTES: See OSDev links in .h file to understand macros
 */ 
void RTC_interrupt(){
    outb(REGISTER_C, RTC_PORT);	    // select register C
    inb(CMOS_PORT);		            // just throw away contents
    RTC_int = 1;                    // RTC interupt has occured
    
    send_eoi(RTC_IRQ);
    // Possible space to put test_interrupts() function.
    test_interrupts();
}

