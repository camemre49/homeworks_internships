#ifndef FILE_HW3_DIRECTORY_TABLE_HANDLER_H
#define FILE_HW3_DIRECTORY_TABLE_HANDLER_H

#include "ext2fs.h"

void set_inode_read_direct_blocks(ext2_inode* inode, uint32_t inode_number);
void deallocate_file_content();
void read_directory_entries();
void print_directory_structure();
void set_inode_read_direct_blocks_no_print(ext2_inode* inode, uint32_t inode_number);
bool is_directory_first_entry(ext2_inode inode, uint32_t inode_number, uint32_t* get_parent_dir);
bool is_directory_block(char* block_content);
bool set_first_directory_pointer_of_inode(char * block_content, uint32_t block_number);
uint32_t get_parent_directory_inode_number_by_directory_block(char* block_content);

#endif //FILE_HW3_DIRECTORY_TABLE_HANDLER_H
