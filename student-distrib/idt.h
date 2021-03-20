/* idt.h (MP3.1) - Defines for necessary functions and vars for IDT initialization
 *         Note that the ifndef-endif wrapping combined with the define is equivalent to a pragma once
 * vim:ts=4 noexpandtab
 */

#ifndef _IDT_H
#define _IDT_H

extern void exception_handler(uint32_t interrupt_vector);

/*
* implement assembly linkage for all relevant exceptions 
* by declaring assembly code from idt.S
*/
extern void divide_by_zero();       //exception 0
extern void debug();                //exception 1
extern void nm_interrupt();         //exception 2
extern void breakpoint();           //exception 3
extern void overflow();             //exception 4
extern void br_exceeded();          //exception 5
extern void inv_opcode();           //exception 6
extern void device_na();            //exception 7
extern void double_fault();         //exception 8
extern void cp_seg_overrun();       //exception 9
extern void inv_tss();              //exception 10
extern void seg_not_present();      //exception 11
extern void stack_fault();          //exception 12
extern void gen_protection();       //exception 13
extern void page_fault();           //exception 14
extern void fpu_floating_point();   //exception 16 (exception 15 is reserved by Intel)
extern void alignment_check();      //exception 17
extern void machine_check();        //exception 18
extern void simd_floating_point();  //exception 19


extern void init_IDT();


#endif /* _IDT_H */
