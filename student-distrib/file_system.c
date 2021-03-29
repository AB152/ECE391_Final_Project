#include "file_system.h"
#include "lib.h"


void init_filesystem(uint32_t start){
    boot=(boot_block_t*)start;      //points to starting memory block of file system 
    fs_inode=(inode_t*)(start+BLOCK_SIZE);   //inodes start one block (4KB) after start/boot
    fs_dentry=(dentry_t*)(start+64);   //dir entries start 64B after start/boot 
    fs_data_block=(data_block_t*)(start+BLOCK_SIZE*(boot->num_inodes+1)); //data block starts a block after inode
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    if(fname==NULL||dentry==NULL)   //check for invalid pointers
        return -1;

    int name_length=strlen(fname);
    if(name_length>FNAME_LENGTH)      //check if name length goes beyond limit
        return -1;
    
    int i;
    for(i=0;i<boot->num_dentries;i++){      //loop through all dentries
        /*if names match, copy over dentry file name, type, and index node into dentry block*/
        if(strncmp(&(boot->dentries[i]),fname,FNAME_LENGTH)){     
            strncpy(dentry->fname, boot->dentries[i].fname,FNAME_LENGTH);
            dentry->ftype=boot->dentries[i].ftype;
            dentry->inode=boot->dentries[i].inode;
            return 0;   //successfully copied over, return 0
        }
    }
    return -1;  //dentry not found, return -1 
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    if(dentry==NULL)   //check for invalid pointer
        return -1;

    if(index >= boot->num_dentries)    //check if index is out of bounds
        return -1;

    /*index is valid, so copy over dentry file name, type, and index node into dentry block*/
    strncpy(dentry->fname, boot->dentries[index].fname,FNAME_LENGTH);
    dentry->ftype=boot->dentries[index].ftype;
    dentry->inode=boot->dentries[index].inode;
    
    return 0;   //successfully copied over, return 0
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    if(buf==NULL)           //check for invalid pointer
        return -1;

    if(inode >= boot->num_inodes)      //check if index node is out of bounds
        return -1;
    
    inode_t* curr_inode=(inode_t*)(&(fs_inode[inode])); //points to the file with inode number inode

    if(offset >= curr_inode->file_size) //check if offset from start of file is out of bounds
        return -1;

    int bytes_read;
    data_block_t* curr_data_block;

    for(bytes_read=0; bytes_read<length; bytes_read++){     //loop through bytes that need to be read
        if((offset+bytes_read) > curr_inode->file_size)     //break if bytes read go out of file bounds
            break;
        curr_data_block=(data_block_t*)(fs_data_block + (curr_inode->index_num[offset/BLOCK_SIZE])); //finds start of data block of which to read bytes from
        buf[bytes_read]=curr_data_block->block[offset%BLOCK_SIZE];  //copy data into buf
        offset++;
    }
    return bytes_read;
}

int32_t read_file(uint32_t inode, uint32_t offset, void* buf, int32_t nbytes){
    return read_data(inode, offset, (uint8_t*)buf, nbytes);
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

int32_t read_dir(uint32_t inode, uint32_t offset, uint8_t* buf, int32_t nbytes){
    if(buf==NULL)       //check for null pointer
        return -1;
    
    int bytes_read;
    for(bytes_read=0; bytes_read<nbytes; bytes_read++){     //loop through bytes that need to be read
        if((offset/FNAME_LENGTH) > boot->num_dentries)        //check if curr directory goes out of bounds
            break;
        buf[bytes_read]=boot->dentries[offset/FNAME_LENGTH].fname[offset%FNAME_LENGTH]; //copy over file name into buf
        offset++;
    }
    return bytes_read;
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

