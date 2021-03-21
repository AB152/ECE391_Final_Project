/* idt.h (MP3.1) - Defines for necessary functions and vars for IDT initialization
 *         Note that the ifndef-endif wrapping combined with the define is equivalent to a pragma once
 * vim:ts=4 noexpandtab
 */

#ifndef _IDT_H
#define _IDT_H

#include "lib.h"
#include "x86_desc.h"
//#ifndef ASM

#include "types.h"

void exception_handler(int32_t interrupt_vector);


void init_IDT();


//#endif /* ASM */
#endif /* _IDT_H */
