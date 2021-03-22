/* paging.h (MP3.1): Header file for paging.c
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "x86_desc.h"


// (MP3.1) Page directory
page_dir_desc_t page_directory[1024] __attribute__((aligned (FOUR_KB)));
// (MP3.1) Page table
page_tab_desc_t page_table_one[1024] __attribute__((aligned (FOUR_KB)));

// Function to initialize paging
extern void init_paging(void);

/* 
 * These might be for the next checkpoint?:
 * ADD FUNCTION TO INTERFACE WITH THE PAGE DIRECTORY
 * ADD FUNCTION TO INTERFACE WITH THE PAGE TABLES
 */

// Function to flush TLB
extern void flush_tlb(void);

#endif /* _PAGING_H */
