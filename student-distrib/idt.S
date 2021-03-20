/* idt.S (MP3.1) : Contains assembly code that handles interrupts */
# vim:ts=4 noexpandtab

#include "idt.h"

divide_by_zero: #0
    pushal
    pushl $0x00
    jmp processor

debug: #1
    pushal
    pushl $0x01
    jmp processor

nm_interrupt: #2
    pushal
    pushl $0x02
    jmp processor

breakpoint: #3
    pushal
    pushl $0x03
    jmp processor

overflow: #4
    pushal
    pushl $0x04
    jmp processor

br_exceeded: #5
    pushal
    pushl $0x05
    jmp processor

inv_opcode: #6
    pushal
    pushl $0x06
    jmp processor

device_na: #7
    pushal
    pushl $0x07
    jmp processor

double_fault: #8
    pushal
    pushl $0x08
    jmp processor

cp_seg_overrun: #9
    pushal
    pushl $0x09
    jmp processor

inv_tss: #10
    pushal
    pushl $0x0A
    jmp processor

seg_not_present: #11
    pushal
    pushl $0x0B
    jmp processor

stack_fault: #12
    pushal
    pushl $0x0C
    jmp processor

gen_protection: #13
    pushal
    pushl $0x0D
    jmp processor

page_fault: #14
    pushal
    pushl $0x0E
    jmp processor

fpu_floating_point: #16
    pushal
    pushl $0x10
    jmp processor

alignment_check: #17
    pushal
    pushl $0x11
    jmp processor

machine_check: #18
    pushal
    pushl $0x12
    jmp processor

simd_floating_point: #19
    pushal
    pushl $0x13
    jmp processor

processor:                      #
    call exception_handler
    addl $4, %esp
    popal
    iret
