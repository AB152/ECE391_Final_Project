/* idt.S (MP3.1) : Contains assembly code that handles interrupts */
# vim:ts=4 noexpandtab

#define ASM     1

#include "idt.h"

.globl divide_by_zero
.globl debug
.globl interrupt
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
.globl processor

divide_by_zero: #0
    pushal              #saves all registers onto stack
    pushf               #save flags
    pushl $0xFFFFFFFF         #prep for exception_handler arg
    movl %esp, %eax
    jmp processor

debug: #1               #for the rest of the exceptions, check DBZ exception for comments
    pushal
    pushf               #save flags
    pushl $0xFFFFFFFE
    movl %esp, %eax
    jmp processor

nm_interrupt: #2
    pushal
    pushf               #save flags
    pushl $0xFFFFFFFD
    movl %esp, %eax
    jmp processor

breakpoint: #3
    pushal
    pushf               #save flags
    pushl $0xFFFFFFFC
    movl %esp, %eax
    jmp processor

overflow: #4
    pushal
    pushf               #save flags
    pushl $0xFFFFFFFB
    movl %esp, %eax
    jmp processor

br_exceeded: #5
    pushal
    pushf               #save flags
    pushl $0xFFFFFFFA
    movl %esp, %eax
    jmp processor

inv_opcode: #6
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF9
    movl %esp, %eax
    jmp processor

device_na: #7
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF8
    movl %esp, %eax
    jmp processor

double_fault: #8
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF7
    movl %esp, %eax
    jmp processor

cp_seg_overrun: #9
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF6
    movl %esp, %eax
    jmp processor

inv_tss: #10
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF5
    movl %esp, %eax
    jmp processor

seg_not_present: #11
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF4
    movl %esp, %eax
    jmp processor

stack_fault: #12
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF3
    movl %esp, %eax
    jmp processor

gen_protection: #13
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF2
    movl %esp, %eax
    jmp processor

page_fault: #14
    pushal
    pushf               #save flags
    pushl $0xFFFFFFF0
    movl %esp, %eax
    jmp processor

fpu_floating_point: #16
    pushal
    pushf               #save flags
    pushl $0xFFFFFFEF
    movl %esp, %eax
    jmp processor

alignment_check: #17
    pushal
    pushf               #save flags
    pushl $0xFFFFFFEE
    movl %esp, %eax
    jmp processor

machine_check: #18
    pushal
    pushf               #save flags
    pushl $0xFFFFFFED
    movl %esp, %eax
    jmp processor

simd_floating_point: #19
    pushal
    pushf               #save flags
    pushl $0xFFFFFFEC
    movl %esp, %eax
    jmp processor

processor:                      #
    call exception_handler
    addl $4, %esp       #clear arg from stack
    /* ret_from_intr below*/
    popf                #restore all registers and flags
    popal
    iret
