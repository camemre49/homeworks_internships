
#ifndef FILE_HW3_POINTER_RECOVERY_HANDLER_H
#define FILE_HW3_POINTER_RECOVERY_HANDLER_H
#include "vector"
#include "ext2fs.h"
#include "inode_bitmap_handler.h"
#include "global_vars.h"
#include "data_bitmap_handler.h"
#include "directory_table_handler.h"

enum Block_Mode {
    DIRECTORY,
    FILE_CONTENT,
    POINTER
};

enum Pointer_Block_Type {
    NOT_POINTER_BLOCK,
    INDIRECT,
    DOUBLE_INDIRECT,
    TRIPLE_INDIRECT
};


struct block_descriptor {
    uint32_t block_number;
    Block_Mode block_content_type;
    uint32_t block_content_size;
    uint32_t pointing_size;
    Block_Mode pointing_structure;
    Pointer_Block_Type pointer_block_type; // If file is pointer, otherwise it is NOT_POINTER_BLOCK
    bool is_full;
};



struct deficient_inode {
    ext2_inode inode;
    uint32_t inode_number;
    uint32_t missing_block_number;
    short missing_pointer_indexes[12]; // 0-12 direct
    bool is_indirect_missing;
    bool is_double_indirect_missing;
    bool is_triple_indirect_missing;
};


void add_orphan_block_group_descriptor(uint32_t block_number, Block_Mode block_mode, char* block_content);
void nullify_orphan_block_group_descriptor(uint32_t block_number);
void print_bgd_vector();
void read_inodes_with_pointer_directory_block_deficiency();
void print_deficient_inodes();
void calculate_orphan_block_sizes_and_correct_inodes();

#endif //FILE_HW3_POINTER_RECOVERY_HANDLER_H
