#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "types.h"


typedef struct{ 
    uint8_t block[4096];    //file system memory is divided into 4KB blocks
}data_block;

//
typedef struct{
    uint32_t index_num[1023];   //holds indices for 1KB of a data block
    uint32_t file_size;         //described in bytes, used for regular files
}inodes;

//first block in file system memory
typedef struct{
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[52];   //52B reserved in boot block, Appendix A
    dentry_t dentries[64]; //64B dir entries in boot block, Appendix A
}boot_block;

typedef struct{
    uint8_t fname[32]; //each name consists of up to 32 characters
    uint32_t ftype;
    uint32_t inode;
    uint8_t reserved[24]; //24B reserved in dir entries, Appendix A
}dentry_t;

extern void init_filesystem();

/*these file system functions are specified in Appendix A*/
extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/*required system call functions from Appendix B*/
extern int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write_file(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open_file(const uint8_t* filename);
extern int32_t close_file (int32_t fd);

extern int32_t read_dir(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write_dir(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open_dir(const uint8_t* dirname);
extern int32_t close_dir (int32_t fd);

#endif /* _FILE_SYSTEM_H */