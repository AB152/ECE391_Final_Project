#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "types.h"

// File descriptor struct for entries in the file descriptor array (FDA)
typedef struct file_descriptor_t {
    uint32_t * fops_table_ptr;
    int32_t inode;
    int32_t file_pos;
    uint32_t flags;
} file_descriptor_t;

/*required functions for CP3, function formats in Appendix B*/
int32_t halt(uint8_t status);

int32_t execute(const uint8_t* command);

int32_t read(int32_t fd, void* buf, int32_t nbytes);

int32_t write(int32_t fd, const void* buf, int32_t nbytes);

int32_t open(const uint8_t* filename);

int32_t close(int32_t fd);

#endif /* _SYSTEM_CALLS_H */
