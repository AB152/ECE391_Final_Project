/* idt.h - Defines for necessary functions and vars for IDT initialization
 *         Note that the ifndef-endif wrapping combined with the define is equivalent to a pragma once
 * vim:ts=4 noexpandtab
 */

#ifndef _IDT_H
#define _IDT_H

extern void exception_handler(uint32_t interrupt_vector);

extern void init_IDT();


#endif /* _IDT_H */
