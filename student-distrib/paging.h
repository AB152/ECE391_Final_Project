/* paging.h (MP3.1): Header file for paging.c
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "x86_desc.h"

/* This is the page directory index of 128MB, which is where we'll put the 4MB user page
   Each page directory entry corresponds to 4MB of VirtMem, so 128 / 4 = 32 */
#define USER_PAGE_BASE_ADDR 32

// (MP3.1) Page directory
page_dir_desc_t page_directory[1024] __attribute__((aligned (FOUR_KB)));
// (MP3.1) Page table
page_tab_desc_t page_table_one[1024] __attribute__((aligned (FOUR_KB)));

// Function to initialize paging
extern void init_paging(void);

// Helper function to set up user page
extern void set_user_page(uint32_t pid, int32_t present_flag);

// Function to flush TLB
extern void flush_tlb(void);

#endif /* _PAGING_H */
