#ifndef FILE_HW3_GLOBAL_VARS_H
#define FILE_HW3_GLOBAL_VARS_H

#include <cstdint>
#include <fstream>
#include "ext2fs.h"
#include <bitset>

extern std::ifstream FILE_IMAGE;
extern uint8_t*  IDENTIFIER;
extern char* IMAGE_PATH;
extern ext2_super_block* SUPER_BLOCK;
extern uint32_t SYSTEM_BLOCK_SIZE;
extern uint32_t SYSTEM_INODE_PER_GROUP;
extern uint32_t SYSTEM_INODE_TABLE_SIZE;
extern uint32_t SYSTEM_INODE_BITMAP_SIZE;
extern uint32_t SYSTEM_DATA_BLOCK_BITMAP_SIZE;
extern uint32_t SYSTEM_BLOCK_GROUP_SIZE;
extern uint32_t SYSTEM_BLOCK_GROUP_COUNT;
extern uint32_t SYSTEM_POINTER_PER_BLOCK;
extern ext2_block_group_descriptor* BLOCK_GROUP_DESCRIPTORS;
extern uint32_t BOOT_AND_SUPER_BLOCK_OFFSET;
extern char ** BLOCK_GROUP_INODE_BITMAPS;
extern char ** BLOCK_GROUP_DATA_BITMAPS;
extern ext2_inode *LAST_READ_INODE_TABLE;
extern ext2_inode ROOT_INODE;

void parse_super_block();
void read_file_image(const char* image_path);
void read_super_block();
uint32_t get_group_start_byte_by_index(int group_index);
void get_group_descriptor_table_by_index(int group_index);
void deallocate_group_descriptors();
void allocate_bitmaps();
void read_bitmaps();
void deallocate_bitmaps();
void read_inode_tables_by_group_index_no_print(int group_index);

#endif // FILE_HW3_GLOBAL_VARS_H
