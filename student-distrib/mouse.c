#include "mouse.h"
#include "asm_linkage.h"
#include "i8259.h"
#include "lib.h"

volatile int8_t mouse_x=0;     //keeps track of mouse x coordinates
volatile int8_t mouse_y=0;     //keeps track of mouse y coordinates


void mouse_wait_data(){              
    uint32_t end_time = 100000;
    while(end_time--){              //wait before input bytes can be read
        if(inb(MOUSE_PS2_PORT)&1){  //first bit must be set
            return;
        }
    }
}

void mouse_wait_signal(){
    uint32_t end_time = 100000;
    while(end_time--){              //wait before sending each output byte
        if((inb(MOUSE_PS2_PORT)&2)==0){ //second bit must be clear
            return;
        }
    }
}

void mouse_write(uint8_t data){
    mouse_wait_signal();                //wait to send data to mouse port
    outb(MOUSE_DATA, MOUSE_PS2_PORT);

    mouse_wait_signal();                //wait to send data to packet
    outb(data, MOUSE_PACKET_PORT);
}

uint8_t mouse_read(){
    mouse_wait_data();                  //wait before accepting data from mouse packet port
    return inb(MOUSE_PACKET_PORT);  
}

void init_mouse(){  //refer to OSdev
    uint8_t status;
    mouse_wait_signal();
    outb(MOUSE_PS2_PORT, MOUSE_AUXILIARY_DEV);  //enable auxiliary mouse device
    
    mouse_wait_signal();
    outb(MOUSE_COMPAQ_STATUS, MOUSE_PS2_PORT);  //sending command byte to mouse port
    mouse_wait_data();
    status = (inb(MOUSE_PACKET_PORT) | 2);      //initialize status byte to enable IRQ12
    status &= 0xDF;                             //clearing bit 6 1101 1111 to disable mouse clock
    mouse_wait_signal();
    outb(MOUSE_PACKET_PORT, MOUSE_PS2_PORT);
    mouse_wait_signal();
    outb(status, MOUSE_PACKET_PORT);

    mouse_write(MOUSE_DEFAULT); //initialize default settings for mouse
    mouse_read();               
    
    mouse_write(MOUSE_PACKET_STREAMING);    //enable mouse
    mouse_read();

    enable_irq(MOUSE_IRQ);

}


void mouse_handler(){
    mouse.val[0]=inb(MOUSE_PACKET_PORT);    //initialize first packet byte
    mouse.val[1]=inb(MOUSE_PACKET_PORT);    //initialize second packet byte, x_movement
    mouse.val[2]=inb(MOUSE_PACKET_PORT);    //initialize third packet byte, y_movement

    if(mouse.x_overflow || mouse.y_overflow || !mouse.reserved){   //if x or y overflows, discard entire packet
        send_eoi(MOUSE_IRQ);
        return;
    }
    mouse_x=mouse.val[1];       //store mouse x movement in global variable
    mouse_y=mouse.val[2];       //store mouse y movement in global variable
    // printf("Mouse x: %d\n", mouse_x);
    // printf("Mouse y: %d\n", mouse_y);
    mouse_cursor();
    send_eoi(MOUSE_IRQ);
}
int previous=0;
static uint8_t temp=' ';


void mouse_cursor(){
    static char* video_mem = (char *)VIDEO;
    // double actual_x=mouse_x;
    // double actual_y=mouse_y;
    
    if(mouse_x<0)
        mouse_x=0;
    if(mouse_y<0)
        mouse_y=0;
    if(mouse_x>=80)
        mouse_x=79;
    if(mouse_y>=25)
        mouse_y=24;
    
    int coordinates = (mouse_y*80) + mouse_x;
    //char temp2=*(uint8_t*)(video_mem + (coordinates << 1));

    *(uint8_t*)(video_mem + (previous << 1))=temp;
    *(uint8_t*)(video_mem + (previous << 1)+1)=ATTRIB;
    temp=*(uint8_t*)(video_mem + (coordinates << 1));
    *(uint8_t*)(video_mem + (coordinates << 1)) = ' ';
    *(uint8_t*)(video_mem + (coordinates << 1)+1) = 0xA7;

    previous=coordinates;
    
}


