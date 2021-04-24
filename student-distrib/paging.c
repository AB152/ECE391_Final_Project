/* paging.c (MP3.1): Functions used to set up page directory and page tables
 * vim:ts=4 noexpandtab
 */

/* Some links to check out: 
 * (https://wiki.osdev.org/Setting_Up_Paging)
 * (https://wiki.osdev.org/Paging)
 * (https://courses.engr.illinois.edu/ece391/sp2021/secure/references/descriptors.pdf)
 * (https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C)
 * (https://en.wikipedia.org/wiki/Control_register#Control_registers_in_x86_series)
 */


#include "paging.h"
#include "lib.h"

/* NOTES: Kernel already loaded at FOUR_MB and should be a single 4MB page.
          VidMem already loaded at VIDEO (see paging.h) and should be a single 4KB page.
          For MP3.1, every page but the VidMem and Kernel pages should be "Not Present" 
          Kyle said something about flushing the TLB? */

/*
 * init_paging
 *    DESCRIPTION: Sets the necessary register values for the processor to do paging
 *    INPUTS/OUTPUTS: none
 *    SIDE EFFECTS: Prepares page directory and page table and enables paging
 */
void init_paging() {
    
    int i;

    for(i=0;i < ONE_KB;i++){
        //fill all of directory w/ blank pages b/c unused
        page_directory[i].pd_mb.present = 0; 
        page_directory[i].pd_mb.read_write = 1; //left as 1 b/c all pages are marked read/write for mp3
        page_directory[i].pd_mb.user_supervisor = 1;
        page_directory[i].pd_mb.page_write_through = 0;
        page_directory[i].pd_mb.page_cache_disabled = 1;
        page_directory[i].pd_mb.accessed = 0;
        page_directory[i].pd_mb.dirty = 0;
        page_directory[i].pd_mb.page_size = 1;   //left 1 to indicate directory is 4mb
        page_directory[i].pd_mb.global_bit = 0;
        page_directory[i].pd_mb.available = 0;
        page_directory[i].pd_mb.page_attr_index = 0;
        page_directory[i].pd_mb.reserved = 0;
        page_directory[i].pd_mb.base_addr = i;
    }      

    // Blank/populate page table
    for(i=0; i<ONE_KB; i++){
        page_tab_desc_t page;
        
        page.present = 0;
        page.read_write = 1;
        page.user_supervisor = 1;
        page.page_write_through = 0;
        page.page_cache_disabled = 0;
        page.accessed = 0;
        page.dirty = 0;
        page.page_attr_tab_index = 0;
        page.global_bit = 0;
        page.avail = 0;
        page.page_base_address = i;

        // Place blank entry in user video table (placed here to avoid next if case)
        user_video_table[i] = page;
        
        // Mark page table entries for kernel vidmem and the 3 terminals as present
        if(i >= VIDMEM_PAGE_BASE && i <= VIDMEM_PAGE_BASE + 3) {
            page.present = 1;   
        }   
        
        // Place entry in kernel video memory page table 
        page_table_one[i] = page;
    }

    // Initializes the memory belonging to the video memory (4 KB within the first 4 MB page)
    page_directory[0].pd_kb.present = 1;        //present b/c page is being initialized
    page_directory[0].pd_kb.read_write = 1;     //all pages are marked read/write for mp3
    page_directory[0].pd_kb.user_supervisor = 0;    //0 for kernel pages
    page_directory[0].pd_kb.page_write_through = 0; //we always want writeback, so 0
    page_directory[0].pd_kb.page_cache_disabled = 0; //0 for video memory pages
    page_directory[0].pd_kb.accessed = 0;   //not used at all in mp3
    page_directory[0].pd_kb.reserved = 0;   //all reserved bits should be set to 0
    page_directory[0].pd_kb.page_size = 0;  //0 if 4K page directory entry
    page_directory[0].pd_kb.global_bit = 0; //0 b/c not kernel page
    page_directory[0].pd_kb.available = 0;  //not used at all in mp3
    page_directory[0].pd_kb.page_table_addr = (unsigned)page_table_one >> 12; //shift address of table so it's 4KB aligned

    // Initializes the memory belonging to the kernel (4 MB - 8 MB)
    page_directory[1].pd_mb.present = 1;    //present b/c page is being initialized
    page_directory[1].pd_mb.read_write = 1;     //all pages are marked read/write for mp3
    page_directory[1].pd_mb.user_supervisor = 0;    //0 for kernel pages
    page_directory[1].pd_mb.page_write_through = 0;    //we always want writeback, so 0
    page_directory[1].pd_mb.page_cache_disabled = 1;    //1 for program code and data pages (kernel pages)
    page_directory[1].pd_mb.accessed = 0;   //not used at all in mp3
    page_directory[1].pd_mb.dirty = 0;      //not used at all in mp3
    page_directory[1].pd_mb.page_size = 1;  //1 if 4M page directory entry
    page_directory[1].pd_mb.global_bit = 1; // the kernel is mapped into every process’s address space
    page_directory[1].pd_mb.available = 0;  //not used at all in mp3
    page_directory[1].pd_mb.page_attr_index = 0;  //not used at all in mp3
    page_directory[1].pd_mb.reserved = 0;       //reserved bits are always set to 0
    page_directory[1].pd_mb.base_addr = 1;      // Kernel is at first 4MB

    for(i=2;i<ONE_KB;i++){
        page_directory[i].pd_mb.present = 0; 
        page_directory[i].pd_mb.read_write = 1; //left as 1 b/c all pages are marked read/write for mp3
        page_directory[i].pd_mb.user_supervisor = 0;
        page_directory[i].pd_mb.page_write_through = 0;
        page_directory[i].pd_mb.page_cache_disabled = 0;
        page_directory[i].pd_mb.accessed = 0;
        page_directory[i].pd_mb.dirty = 0;
        page_directory[i].pd_mb.page_size = 1;   //left 1 to indicate directory is 4mb
        page_directory[i].pd_mb.global_bit = 0;
        page_directory[i].pd_mb.available = 0;
        page_directory[i].pd_mb.page_attr_index = 0;
        page_directory[i].pd_mb.reserved = 0;
        page_directory[i].pd_mb.base_addr = i;
    }       

    /* Flush the TLB as we've made changes to the paging structure */
    flush_tlb();

    // Load and enable page directory address in register CR3
    asm volatile ("movl  %0, %%eax;"                //loads page directory
                  "movl %%eax, %%cr3;"
                  "movl %%cr4, %%eax;"              
                  "orl $0x00000010, %%eax;"         
                  "movl %%eax, %%cr4;"
                  "movl %%cr0, %%eax;"              //enables page directory
                  "orl $0x80000000, %%eax;"
                  "movl %%eax, %%cr0;"
                :                                   // no outputs
                : "r" (page_directory)              // input: page_directory
                : "eax", "cc"        // clobbers eax and condition codes
    );

}

/*  
 * set_user_page
 *    DESCRIPTION: Re-maps the user program page for the process with pid
 *    INPUTS: pid -- ID of process
 *            present_flag -- set to 0 to mark page not present, 1 to mark as present
 *    RETURNS: none  
 *    SIDE EFFECTS: Sets up user page in PhysMem
 *    NOTES: 
 */
void set_user_page(uint32_t pid, int32_t present_flag) {
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.present = present_flag;
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.read_write = 1;     //all pages are marked read/write for mp3
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.user_supervisor = 1;    //1 for user pages
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.page_write_through = 0;    //we always want writeback, so 0
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.page_cache_disabled = 1;    //1 for program code and data pages (kernel pages)
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.accessed = 0;   //not used at all in mp3
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.dirty = 0;      //not used at all in mp3
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.page_size = 1;  //1 if 4M page directory entry
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.global_bit = 0; // user page should not be global
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.available = 0;  //not used at all in mp3
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.page_attr_index = 0;  //not used at all in mp3
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.reserved = 0;       //reserved bits are always set to 0
    page_directory[USER_PAGE_BASE_ADDR].pd_mb.base_addr = 2 + pid;     // Map physmem [8MB + (pid * 4MB)] as mult of 4MB
    flush_tlb();
}

/*  
 * set_user_video_page
 *    DESCRIPTION: Sets up page for user to interact with video memory
 *    INPUTS: present_flag -- set to 0 to mark page not present, 1 to mark as present
 *    RETURNS: none  
 *    SIDE EFFECTS: Sets up video page for user use
 *    NOTES: 
 */
void set_user_video_page(int32_t present_flag) {
    user_video_table[0].present = present_flag;            // Mark table entry as present
    user_video_table[0].page_base_address = VIDMEM_PAGE_BASE;  // Set page to point to video memory
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.present = present_flag;        //present b/c page is being initialized
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.read_write = 1;     //all pages are marked read/write for mp3
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.user_supervisor = 1;    //1 for user-level pages
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.page_write_through = 0; //we always want writeback, so 0
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.page_cache_disabled = 0; //0 for video memory pages
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.accessed = 0;   //not used at all in mp3
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.reserved = 0;   //all reserved bits should be set to 0
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.page_size = 0;  //0 if 4K page directory entry
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.global_bit = 0; //0 b/c not kernel page
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.available = 0;  //not used at all in mp3
    page_directory[USER_VID_PAGE_DIR_I].pd_kb.page_table_addr = (unsigned)user_video_table >> 12; //shift address of table for 4KB align
    flush_tlb();
}

/*
 * set_active_terminal_page
 *    DESCRIPTION: Sets up page for user to interact with video memory
 *    INPUTS: from_terminal_id -- the id of the terminal to switch from
 *            to_terminal_id -- the id of the terminal to switch to
 *    RETURNS: none  
 *    SIDE EFFECTS: Changes the kernel video memory page to track the desired terminal's video page
 *    NOTES: 
 */
void change_terminal_video_page(int32_t from_terminal_id, int32_t to_terminal_id) {
    // Bounds check
    if(from_terminal_id < 0 || from_terminal_id > 2 || to_terminal_id < 0 || to_terminal_id > 2)
        return;

    // Save current terminal's screen to its video page
    memcpy((void *)(VIDMEM + (from_terminal_id + 1) * FOUR_KB), (void *)VIDMEM, FOUR_KB);

    // Restore new terminal's screen to video memory
    memcpy((void *)VIDMEM, (void *)(VIDMEM + (to_terminal_id + 1) * FOUR_KB), FOUR_KB);
}

/*  
 * flush_tlb
 *    DESCRIPTION: Used to flush TLB after making a change to the paging structure
 *    INPUTS/OUTPUTS: NONE
 *    SIDE EFFECTS: Flushes TLB for paging
 *    NOTES: See (https://wiki.osdev.org/TLB)
 */ 
void flush_tlb() {
    asm volatile ("movl	%%cr3, %%eax;"
	              "movl	%%eax, %%cr3;"
                :               // no inputs
                :               // no outputs
                : "eax"        // clobbers eax
    );
}
