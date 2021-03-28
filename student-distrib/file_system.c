#include "file_system.h"
#include "lib.h"

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int name_length=strlen(fname);
    if(name_length>32)      //each name can only consist of up to 32 char
        return -1;
    
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

}

int32_t read_file(int32_t fd, void* buf, int32_t nbytes){

}

int32_t write_file(int32_t fd, const void* buf, int32_t nbytes){
    return -1;          //file system is read-only so writing always fails
}

int32_t open_file(const uint8_t* filename){
    return 0;           //always success, no need to check for name validity
}

int32_t close_file (int32_t fd){
    return 0;           //closing is always successful
}

int32_t read_dir(int32_t fd, void* buf, int32_t nbytes){

}

int32_t write_dir(int32_t fd, const void* buf, int32_t nbytes){
    return -1;          //file system is read-only so writing always fails
}

int32_t open_dir(const uint8_t* dirname){
    return 0;           //always success, no need to check for name validity
}

int32_t close_dir (int32_t fd){
    return 0;           //closing is always successful
}

