#include "system_calls.h"
#include "paging.h"
#include "lib.h"
#include "file_system.h"

int32_t halt(uint8_t status){

}

int32_t execute(const uint8_t* command){
    if(command == NULL){
        return;
    }
    int pid; /* IMPORTANT */ // NEED TO DEFINE
    // Check if pid goes above 6 (5 if 0-indexed)
    uint32_t command_length=strlen(command);
    uint8_t exec_name[command_length];
    dentry_t file_dentry;
    set_user_page(pid, 1); // Set present bit in execute and 0 in halt
    int i = 0;
    while(command[i] != NULL && command[i] != ' ' && command[i] != '\n' && i < strlen(command)){
        exec_name[i] = command[i];
        i++;
    }
    if(exec_name != strlen(command)){
        exec_name[i] = NULL;
    }
    int dentry_res = read_dentry_by_name(exec_name, &file_dentry);

    if(dentry_res == -1){
        return -1;
    }


}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
    if(fd<0 || fd>7) //check for valid fd index, max 8 files
        return -1;

    if(buf==NULL)
        return -1;
    
    //question is how do i initialize the pcb??
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    if(fd<0 || fd>7) //check for valid fd index, max 8 files
        return -1;

    if(buf==NULL)
        return -1;
}

int32_t open(const uint8_t* filename){
    if(filename==NULL)
        return -1;
}

int32_t close(int32_t fd){
    if(fd<0 || fd>7) //check for valid fd index, max 8 files
        return -1;
}




