#include "x86_desc.h"
#include "system_calls.h"
#include "paging.h"
#include "lib.h"
#include "rtc.h"
#include "file_system.h"
#include "terminal.h"

/*fops tables for different types*/
fops_jump_table_t rtc_table = {RTC_read, RTC_write, RTC_open, RTC_close};
fops_jump_table_t directory_table = {read_dir, write_dir, open_dir, close_dir};
fops_jump_table_t file_table = {read_file, write_file, open_file, close_file};

fops_jump_table_t stdin_table = {terminal_read,bad_call,bad_call,bad_call};
fops_jump_table_t stdout_table = {bad_call,terminal_write,bad_call,bad_call};

fops_jump_table_t bad_table = {bad_call,bad_call,bad_call,bad_call};

// Array of flags (should they be PCBs?) to track currently running processes
uint32_t processes[6] = {0, 0, 0, 0, 0, 0};

uint32_t last_assigned_pid;

int32_t bad_call(){
    return -1;
}

int32_t halt(uint8_t status){

    cli();
    pcb_t *pcb_ptr =(pcb_t*)(tss.esp0  & 0xFFFFE000);

    if(pcb_ptr->parent_process_id == 0 && pcb_ptr -> parent_esp == NULL && pcb_ptr -> parent_ebp == NULL){
        execute((uint8_t*)"shell");
    }
    //initalize pcb
    int i;
    for(i = 0; i < 8; i++) {
        if(pcb_ptr->fda[i].flags==1){   //close any file descriptors in use
            close(i);   
            pcb_ptr->fda[i].fops_table_ptr=bad_table;
        }    
    }
    
    // restore paging
    set_user_page(pcb_ptr -> parent_process_id, 1);

    tss.esp0 = EIGHT_MB - (pcb_ptr -> parent_process_id * EIGHT_KB) - 4;    //setting ESP0 to base of new kernel stack
    tss.ss0 = KERNEL_DS;    //setting SS0 to kernel data segment

    // swap stacks
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "xorl %%eax, %%eax;"
        "movb %2, %%al;"
        // "pushl %2;"
        // "popl %%eax;"
        "jmp EXECUTE_LABEL;"
        :       // Outputs
        : "r"(pcb_ptr->parent_esp), "r"(pcb_ptr->parent_ebp), "r"(status) // Inputs
        : "eax" // Clobbers
    );
    return -1;      //temp return val
}

int32_t execute(const uint8_t* command){
    
    cli();
    
    // Check null input 
    if(command == NULL){
        return -1;
    }

    // Find next available PID to assign
    int i, next_pid; 
    for(i = 0; i < MAX_PROCESSES; i++) {    //find next available process index
        // If we've found a free PID, mark it and break from loop
        if(processes[i] == 0) {
            next_pid = i;
            break;
        }
        // If we've reached end without finding a free PID, cannot execute
        if(i == MAX_PROCESSES - 1)
            return -1;
    }
    

    // Allocate PCB
    // Calculate pointer to next PCB
    pcb_t * next_pcb_ptr = (pcb_t *)(EIGHT_MB - ((next_pid + 1) * EIGHT_KB));

    // Initialize every fda entry
    for(i = 0; i < 8; i++) {
        next_pcb_ptr->fda[i].inode = 0;
        next_pcb_ptr->fda[i].file_pos = 0;
        next_pcb_ptr->fda[i].flags = 0;
    }

    // Set up fops tables for stdin and stdout respectively in the new pcb
    next_pcb_ptr->fda[0].fops_table_ptr = stdin_table;
    next_pcb_ptr->fda[1].fops_table_ptr = stdout_table;

    // Parse command
    uint32_t command_length=strlen((int8_t *)command);
    uint8_t exec_name[command_length];
    dentry_t file_dentry;

    // Find command from entry
    i = 0;
    while(command[i] != NULL && command[i] != ' ' && command[i] != '\n' && i < strlen((int8_t*)command)){
        exec_name[i] = command[i];
        i++;
    }

    // Add null-terminator
    uint32_t exec_length = strlen((int8_t*)exec_name);
    if(exec_length != strlen((int8_t*)command)){
        exec_name[i] = NULL;        
    }

    // Find file and do executable check
    //check whether file exists within directory
    int dentry_res = read_dentry_by_name(exec_name, &file_dentry);  
    //int invalid_ftype=0;
    if(dentry_res == -1){       
        return -1;
    }

    // Copy program file to allocated page
    // Allocate Page and flush TLB
    set_user_page(next_pid, 1); // Set present bit in execute and 0 in halt
    
    // Load executable into user page
    int val = read_data(file_dentry.inode, 0, (uint8_t*)PROG_IMG_ADDR, exec_length);
    if(val == -1){
        set_user_page(next_pid, 0);
        return -1;
    }

    // Check ELF constant to see if file is an executable
    uint8_t elf_check[4];
    read_data(file_dentry.inode, 0, elf_check, 4);
    if(elf_check[0] != 0x7f){
        set_user_page(next_pid, 0);
        return -1;
    }
    if(elf_check[1] != 0x45){
        set_user_page(next_pid, 0);
        return -1;
    }
    if(elf_check[2] != 0x4c){
        set_user_page(next_pid, 0);
        return -1;
    }
    if(elf_check[3] != 0x46){
        set_user_page(next_pid, 0);
        return -1;
    }

    if(next_pid==0){    //first process
        next_pcb_ptr->parent_process_id=0;
        next_pcb_ptr->process_id=0;
    }
    else{
        next_pcb_ptr->parent_process_id=last_assigned_pid;
        next_pcb_ptr->process_id=next_pid;
    }

    // Mark PID as in use and set PCB
    processes[next_pid] = 1;
    next_pcb_ptr->process_id = next_pid;
    last_assigned_pid = next_pid;

    // Get addr exec's first instruction (bytes 24-27 of the exec file)
    uint8_t prog_entry_buf[4];
    uint32_t prog_entry_addr;
    
    // IMPORTANT: Byte 24 is MSByte, but line 181 reads it backwards. Find a fix? 
    read_data(file_dentry.inode, 24, prog_entry_buf, 4);
    prog_entry_addr = *((uint32_t*)prog_entry_buf);

    // Prepare TSS for context switch
    tss.esp0 = EIGHT_MB - (next_pid * EIGHT_KB) - 4;    //setting ESP0 to base of new kernel stack
    tss.ss0 = KERNEL_DS;    //setting SS0 to kernel data segment

    if(next_pid == 0){
        next_pcb_ptr->parent_esp = NULL;
        next_pcb_ptr->parent_ebp = NULL;
    }
    else{
        // Save state of current stack into PCB
        asm volatile ("movl %%esp, %0;"
                      "movl %%ebp, %1;"
                    : "=r" (next_pcb_ptr->parent_esp), "=r" (next_pcb_ptr->parent_ebp)    // Outputs
        );   
    }

    // Push items to stack and context switch using IRET
    asm volatile (

        //"xorl %%eax, %%eax;"
        "pushl %1;"
        //"popw %%ax;"
        //"pushl %%eax;"

        "pushl $0x083ffffc;"         // Set ESP to point to the user page
        
        //"xorl %%eax, %%eax;"
        "pushfl;"
        "popl %%eax;"
        "orl $0x200, %%eax;"        //sets bit 9 to 1 in the flags register to sti
        "pushl %%eax;"

        //"xorl %%eax, %%eax;"
        "pushl %2;"
        //"popw %%ax;"
        //"pushl %%eax;"

        "pushl %0;"

        "iret;"

        "EXECUTE_LABEL: "
        :                       // No Outputs
        : "r"(prog_entry_addr), "r"(USER_DS), "r"(USER_CS)      // Inputs
        : "eax", "cc"                      // Clobbers
    );
    

    return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
    /* IMPORTANT */
    pcb_t *pcb = (pcb_t*)(tss.esp0  & 0xFFFFE000); //ANDing the process's ESP register w/ appropriate bit mask to reach top of stack
    if(fd<0 || fd>7) //check for valid fd index, max 8 files
        return -1;

    if(buf==NULL)
        return -1;
    
    //question is how do i initialize the pcb??

    return pcb->fda[fd].fops_table_ptr.read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    pcb_t *pcb=(pcb_t*)(tss.esp0  & 0xFFFFE000);    //temp placeholder until we figure out how to initialize the pcb
    if(fd<0 || fd>7) //check for valid fd index, max 8 files
        return -1;

    if(buf==NULL)
        return -1;
    
    return pcb->fda[fd].fops_table_ptr.write(fd, buf, nbytes);
}

int32_t open(const uint8_t* filename){
    pcb_t *pcb=(pcb_t*)(tss.esp0  & 0xFFFFE000);    //temp placeholder until we figure out how to initialize the pcb
    if(filename==NULL)  //check for valid file
        return -1;

    dentry_t dentry;
    if(read_dentry_by_name(filename,&dentry)==-1)   //check if file exists within dentry
        return -1;
    

    /*find an available space in file descriptor array, 
    * 0 and 1 indices are reserved for stdin and stdout
    * max 8 open files at a time
    */
    uint32_t i;
    uint32_t available=0;       //flags whether available space is found
    /* IMPORTANT */ // Ask if we need to include stdin and stdout below
    for(i=2; i<8; i++){         
        if(pcb->fda[i].flags==0){   //if available space is found, initialize file descriptor
            pcb->fda[i].file_pos=0; //should always start at beginning of file
            pcb->fda[i].flags=1;    //mark space as used
            available=1;            
            break;
        }
    }

    if(available==0)             //if no available space is found, fail
        return -1;
    
    uint32_t file_type = dentry.ftype;
    if(file_type==0){   //ftype 0 for RTC
        pcb->fda[i].fops_table_ptr=rtc_table;
    }
    else if(file_type==1){   //ftype 1 for directory
        pcb->fda[i].fops_table_ptr=directory_table;
    }
    else if(file_type==2){   //ftype 2 for regular file
        pcb->fda[i].fops_table_ptr=file_table;
        pcb->fda[i].inode=dentry.inode;
    }

    return i;                   //return index of file assigned in file descriptor array
}

int32_t close(int32_t fd){
    pcb_t* pcb=(pcb_t*)(tss.esp0  & 0xFFFFE000); //temp placeholder until we figure out how to initialize the pcb
    
    if(fd<2 || fd>7) //check for valid fd index, max 8 files
        return -1;
    
    pcb->fda[fd].flags=0; //mark index in file descriptor array as available
    
    return pcb->fda[fd].fops_table_ptr.close(fd);
}




