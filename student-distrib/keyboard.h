
// http://www.philipstorr.id.au/pcbook/book3/scancode.htm

#define KEYBOARD_PORT           0x60
#define KEYBOARD_IRQ            0x01
#define LEFT_SHIFT_PRESSED      0x2A
#define RIGHT_SHIFT_PRESSED     0x36
#define LEFT_SHIFT_RELEASED     0xAA
#define RIGHT_SHIFT_RELEASED    0xB6
#define CAPS_LOCK_PRESSED       0x3A
#define LEFT_CTRL_PRESSED       0x1D
#define LEFT_CTRL_RELEASED      0x9D
#define LEFT_ALT_PRESSED        0x38
#define LEFT_ALT_RELEASED       0xB8
#define TAB_PRESSED             0x0F
#define TAB_RELEASED            0x8F
#define ENTER_PRESSED           0x1C
#define ENTER_RELEASED          0x9C
#define BACKSPACE_PRESSED       0x0E
#define BACKSPACE_RELEASED      0x8E
#define KEYBOARD_BUF_SIZE       128

// Line buffer for keyboard entries
char keyboard_buf[KEYBOARD_BUF_SIZE];

// Keyboard buffer index to keep track of buffer pos
int keyboard_buf_i;

// Keyboard flags
int left_shift_flag;

int right_shift_flag;

int ctrl_flag;

int caps_flag;

int alt_flag;

int tab_flag;

int enter_flag;

int backspace_flag;

// Initialize the keyboard by enabling the PIC IRQ
void init_keyboard();
