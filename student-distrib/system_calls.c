#include "system_calls.h"
#include "paging.h"

int32_t halt(uint8_t status){

}

int32_t execute(const uint8_t* command){
    if(command == NULL){
        return;
    }
    int pid; /* IMPORTANT */ // NEED TO DEFINE
    // Check if pid goes above 6 (5 if 0-indexed)
    uint8_t exec_name[strlen(command)];
    dentry_t file_dentry;
    set_user_page(pid, 1); // Set present bit in execute and 0 in halt
    int i = 0;
    while(command[i] != NULL && command[i] != ' ' && command[i] != '\n' && i < strlen(command)){
        exec_name[i] = command[i];
        i++;
    }
    if(file_name != strlen(command)){
        exec_name[i] = NULL;
    }
    int dentry_res = read_dentry_by_name(exec_name, &file_dentry);

    if(dentry_res == -1){
        return -1;
    }


}

int32_t read(int32_t fd, void* buf, int32_t nbytes){

}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){

}

int32_t open(const uint8_t* filename){

}

int32_t close(int32_t fd){

}




