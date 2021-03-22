/* paging.h (MP3.1): Header file for paging.c
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "x86_desc.h"

// Define macros for byte sizes
#define ONE_KB 1024             // 1024 bytes make a KB
#define FOUR_MB 0x400000        // Coincidentally, ONE_KB * ONE_KB * 4 == 0x400000, also the kernel is loaded here
#define VIDEO 0xB8000           // Starting address of VidMEM (Copied from lib.c, this shouldn't be a problem)

/* IMPORTANT */
// Must cast the below to uint32_t when dealing with kernel
// Page directory
extern page_dir_4kb_t page_directory[1024] __attribute__((aligned(4 * ONE_KB)));
// Page table
extern uint32_t page_table_one[1024] __attribute__((aligned(4 * ONE_KB)));

// Function to initialize paging
extern void init_paging(void);

// Function to flush TLB
extern void flush_tlb(void);

#endif /* _PAGING_H */
g