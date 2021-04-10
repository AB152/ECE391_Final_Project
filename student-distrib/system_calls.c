#include "x86_desc.h"
#include "system_calls.h"
#include "paging.h"
#include "lib.h"
#include "rtc.h"
#include "file_system.h"
#include "terminal.h"

/*fops tables for different types*/
fops_jump_table_t rtc_table = {&RTC_read,&RTC_write,&RTC_open,&RTC_close};
fops_jump_table_t directory_table = {&read_dir,&write_dir,&open_dir,&close_dir};
fops_jump_table_t file_table = {&read_file,&write_file,&open_file,&close_file};

fops_jump_table_t stdin_table = {&terminal_read,&bad_call,&terminal_open,&terminal_close};
fops_jump_table_t stdout_table = {&bad_call,&terminal_write,&terminal_open,&terminal_close};

// MAKE ARRAY OF PCBS AND GLOBAL VAR FOR PID HERE

int32_t bad_call(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

int32_t halt(uint8_t status){

}

int32_t execute(const uint8_t* command){
    if(command == NULL){
        return -1;
    }
    int pid; /* IMPORTANT */ // NEED TO DEFINE (globally??)
    // Check if pid goes above 6 (5 if 0-indexed)
    uint32_t command_length=strlen((int8_t)command);
    uint8_t exec_name[command_length];
    dentry_t file_dentry;
    set_user_page(pid, 1); // Set present bit in execute and 0 in halt
    int i = 0;

    // Find command from entry
    while(command[i] != NULL && command[i] != ' ' && command[i] != '\n' && i < strlen(command)){
        exec_name[i] = command[i];
        i++;
    }

    // Add null-terminator
    uint32_t exec_length = strlen(exec_name);
    if(exec_length != strlen(command)){
        exec_name[i] = NULL;        
    }

    //check whether file exists within directory
    int dentry_res = read_dentry_by_name(exec_name, &file_dentry);  
    if(dentry_res == -1 || file_dentry.ftype){       
        return -1;
    }

    // Load executable into user page
    read_data(file_dentry.inode, 0, ONE_TWO_EIGHT_MB, exec_length);

    

}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
    pcb_t *pcb=NULL;    //temp placeholder until we figure out how to initialize the pcb
    if(fd<0 || fd>7) //check for valid fd index, max 8 files
        return -1;

    if(buf==NULL)
        return -1;
    
    //question is how do i initialize the pcb??

    return pcb->fda[fd].fops_table_ptr.read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    pcb_t *pcb=NULL;    //temp placeholder until we figure out how to initialize the pcb
    if(fd<0 || fd>7) //check for valid fd index, max 8 files
        return -1;

    if(buf==NULL)
        return -1;
    
    return pcb->fda[fd].fops_table_ptr.write(fd, buf, nbytes);
}

int32_t open(const uint8_t* filename){
    pcb_t *pcb=NULL;    //temp placeholder until we figure out how to initialize the pcb
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
    pcb_t* pcb=NULL; //temp placeholder until we figure out how to initialize the pcb
    
    if(fd<2 || fd>7) //check for valid fd index, max 8 files
        return -1;
    
    pcb->fda[fd].flags=0; //mark index in file descriptor array as available
    
    return pcb->fda[fd].fops_table_ptr.close(fd);
}




