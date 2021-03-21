/* idt.S (MP3.1) : Contains assembly code that handles interrupts */
# vim:ts=4 noexpandtab

#define ASM     1

#include "idt_ex_handler.h"

.text

/*
* make all exception and device processors global to implement
* assembly linkage
*/
.globl divide_by_zero
.globl debug
.globl nm_interrupt
.globl breakpoint
.globl overflow
.globl br_exceeded
.globl inv_opcode
.globl device_na
.globl double_fault
.globl cp_seg_overrun
.globl inv_tss
.globl seg_not_present
.globl stack_fault
.globl gen_protection
.globl page_fault
.globl fpu_floating_point
.globl alignment_check
.globl machine_check
.globl simd_floating_point
.globl exception_processor
.globl keyboard_processor

/*
*ask TA about whether moving esp into eax is valid
*/

divide_by_zero: #0
    pushal              #saves all registers onto stack
    pushfl               #save flags
    pushl $0xFFFFFFFF         #prep for exception_processor arg
    //movl %esp, %eax
    jmp exception_processor

debug: #1               #for the rest of the exceptions, check DBZ exception for comments
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFFE
    //movl %esp, %eax
    jmp exception_processor

nm_interrupt: #2
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFFD
    //movl %esp, %eax
    jmp exception_processor

breakpoint: #3
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFFC
    //movl %esp, %eax
    jmp exception_processor

overflow: #4
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFFB
    //movl %esp, %eax
    jmp exception_processor

br_exceeded: #5
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFFA
    //movl %esp, %eax
    jmp exception_processor

inv_opcode: #6
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF9
    //movl %esp, %eax
    jmp exception_processor

device_na: #7
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF8
    //movl %esp, %eax
    jmp exception_processor

double_fault: #8
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF7
    //movl %esp, %eax
    jmp exception_processor

cp_seg_overrun: #9
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF6
    //movl %esp, %eax
    jmp exception_processor

inv_tss: #10
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF5
    //movl %esp, %eax
    jmp exception_processor

seg_not_present: #11
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF4
    //movl %esp, %eax
    jmp exception_processor

stack_fault: #12
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF3
    //movl %esp, %eax
    jmp exception_processor

gen_protection: #13
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF2
    //movl %esp, %eax
    jmp exception_processor

page_fault: #14
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFF1
    //movl %esp, %eax
    jmp exception_processor

fpu_floating_point: #16
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFEF
    //movl %esp, %eax
    jmp exception_processor

alignment_check: #17
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFEE
    //movl %esp, %eax
    jmp exception_processor

machine_check: #18
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFED
    //movl %esp, %eax
    jmp exception_processor

simd_floating_point: #19
    pushal
    pushfl               #save flags
    pushl $0xFFFFFFEC
    //movl %esp, %eax
    jmp exception_processor

exception_processor:            #passes interrupt vector into exception_handler
    call exception_handler
    addl $4, %esp               #clear arg from stack
    /* ret_from_intr below*/
    popfl                       #restore all registers and flags
    popal
    iret

keyboard_processor:             #once keyboard interrupt occurs, call keyboard handler
    pushal 
    pushfl 
    call keyboard_handler
    popfl 
    popal 
    iret 
