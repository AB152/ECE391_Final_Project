/* paging.c (MP3.1): Functions used to set up page directory and page tables
 * vim:ts=4 noexpandtab
 */

/* Some links to check out: 
 * (https://wiki.osdev.org/Setting_Up_Paging)
 * (https://wiki.osdev.org/Paging)
 * (https://courses.engr.illinois.edu/ece391/sp2021/secure/references/descriptors.pdf)
 * (https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C)
 */

#include "paging.h"


/* NOTES: Kernel already loaded at FOUR_MB and should be a single 4MB page.
          VidMem already loaded at VIDEO (see paging.h) and should be a single 4KB page.
          For MP3.1, everything but the VidMem and Kernel pages should be "Not Present" 
          Kyle said something about flushing the TLB? */

/*
 * init_paging
 *    DESCRIPTION: Sets the necessary register values for the processor to do paging.
 */
void init_paging() {
    
    int i;
    
    for(i=0; i<ONE_KB; i++){
        page_dir_4kb_t page;
        
        page.present = 0;
        page.read_write = 0;
        page.user_supervisor = 0;
        page.page_write_through = 0;
        page.page_cache_disabled = 0;
        page.accessed = 0;
        page.reserved = 0;
        page.page_size = 0;
        page.global_bit = 0;
        page.available = 0;
        page.page_table_addr = i;

        page_directory[i] = page;
    }

    for(i = 0; i < ONE_KB; i++){
        
    }
    
    // Inline ASM to load page directory by setting register CR3
    asm volatile ("pushl %%ebp              \n\
                  movl %%esp, %%ebp         \n\
                  movl 8(%%esp), %%eax      \n\
                  movl %%eax, %%cr3         \n\
                  movl %%ebp, %%esp         \n\
                  popl %%ebp"               \
                : 
    );
    // Enable paging by setting the 32th bit in the CR0 register to 1
    asm volatile ("pushl %%ebp              \n\
                  movl %%esp, %%ebp         \n\
                  movl %%cr0, %%eax         \n\
                  orl $0x80000000, %%eax    \n\
                  movl %%eax, %%cr0         \n\
                  movl %%ebp, %%esp         \n\
                  popl %%ebp"               \
                : 
    );

}


// https://wiki.osdev.org/TLB
void flush_tlb() {
    asm volatile(
        "movl	%%cr3, %%eax"
	    "movl	%%eax, %%cr3"
    );
}
