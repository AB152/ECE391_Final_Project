#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"

#define MOUSE_PS2_PORT          0x64
#define MOUSE_DATA              0xD4
#define MOUSE_AUXILIARY_DEV     0xA8
#define MOUSE_COMPAQ_STATUS     0x20
#define MOUSE_PACKET_PORT       0x60
#define MOUSE_DEFAULT           0xF6
#define MOUSE_PACKET_STREAMING  0xF4
#define MOUSE_IRQ               12

typedef struct{
    union {
        int8_t val[3];
        struct {
            //first packet byte
            uint8_t left_btn      :1;
            uint8_t right_btn     :1;
            uint8_t middle_btn    :1;
            uint8_t reserved      :1;
            int8_t x_signb        :1;
            int8_t y_signb        :1;
            uint8_t x_overflow    :1;
            uint8_t y_overflow    :1;

            //second packet byte
            int8_t x_movement;

            //third packet byte
            int8_t y_movement;
        } __attribute__ ((packed));
        
    };

}mouse_t;

mouse_t mouse;


void mouse_wait_data();

void mouse_wait_signal();

void mouse_write(uint8_t data);

uint8_t mouse_read();

void init_mouse();

void mouse_handler();

#endif /* _MOUSE */
