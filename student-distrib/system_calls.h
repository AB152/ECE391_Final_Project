#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "types.h"

//Appendix A 8.2, fops table should contain entries for open, read, write, and close
//Note: functions are casted to pointers, otherwise C won't recognize them in struct
typedef struct{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
}fops_jump_table;

// File descriptor struct for entries in the file descriptor array (FDA)
typedef struct file_descriptor_t {
    fops_jump_table * fops_table_ptr;      
    int32_t inode;
    int32_t file_pos;
    uint32_t flags;
} file_descriptor_t;

//Process control block (PCB) struct described in Appendix A 8.2
typedef struct{
    file_descriptor_t open_files_array[8];    //up to 8 open files are represented with a file array in PCB

}pcb_t;

/*required functions for CP3, function formats in Appendix B*/
int32_t halt(uint8_t status);

int32_t execute(const uint8_t* command);

int32_t read(int32_t fd, void* buf, int32_t nbytes);

int32_t write(int32_t fd, const void* buf, int32_t nbytes);

int32_t open(const uint8_t* filename);

int32_t close(int32_t fd);

#endif /* _SYSTEM_CALLS_H */
