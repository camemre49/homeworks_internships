#include <cstring>
#include "data_bitmap_handler.h"
#include "ext2fs.h"
#include "global_vars.h"
#include "directory_table_handler.h"
#include "pointer-recovery-handler.h"

char** DATA_BITMAPS_REPLICATE;
bool data_bitmap_combined;
bool data_bitmap_allocated = false;

void allocate_data_bitmap_replicate() {
    if(!data_bitmap_allocated) {
        DATA_BITMAPS_REPLICATE = new char *[SYSTEM_BLOCK_GROUP_COUNT];
        for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
            DATA_BITMAPS_REPLICATE[i] = new char[SYSTEM_DATA_BLOCK_BITMAP_SIZE];
            std::memset(DATA_BITMAPS_REPLICATE[i], 0, SYSTEM_DATA_BLOCK_BITMAP_SIZE); // Initialize to zero
        }
        data_bitmap_allocated = true;
    }
}


void set_data_block_bitmap(uint32_t block_number, bool no_fix) {
    if(block_number > SUPER_BLOCK->block_count){
        return;
    }
    uint32_t boot_block_and_super_block = BOOT_AND_SUPER_BLOCK_OFFSET;
    if(!no_fix && block_number > boot_block_and_super_block){
        block_number -= boot_block_and_super_block;
    }
    uint32_t block_group = block_number  / SUPER_BLOCK->blocks_per_group;
    uint32_t index_in_block_group = block_number % SUPER_BLOCK->blocks_per_group;
    uint32_t bitmap_byte = index_in_block_group / 8;
    uint32_t bitmap_bit = index_in_block_group % 8;
    DATA_BITMAPS_REPLICATE[block_group][bitmap_byte] |= (1 << bitmap_bit);
}

void read_block(uint32_t block_ptr, uint32_t* buffer) {
    if (!FILE_IMAGE.is_open()) {
        return;
    }

    uint32_t byte_offset = block_ptr * SYSTEM_BLOCK_SIZE;
    FILE_IMAGE.seekg(byte_offset);
    FILE_IMAGE.read(reinterpret_cast<char*>(buffer), SYSTEM_BLOCK_SIZE);
}

bool is_set_block_bitmap(uint32_t block_number, bool no_fix = false) {
    if(block_number > SUPER_BLOCK->block_count){
        return false;
    }
    uint32_t boot_block_and_super_block = BOOT_AND_SUPER_BLOCK_OFFSET;
    if(!no_fix){
        block_number -= boot_block_and_super_block;
    }
    int32_t block_group = block_number / SUPER_BLOCK->blocks_per_group;
    uint32_t index_in_block_group = block_number % SUPER_BLOCK->blocks_per_group;
    uint32_t bitmap_byte = index_in_block_group / 8;
    uint32_t bitmap_bit = index_in_block_group % 8;
    return (DATA_BITMAPS_REPLICATE[block_group][bitmap_byte] & (1 << bitmap_bit)) != 0;
}

void set_data_blocks_of_inode_in_bitmap(ext2_inode inode) {
    // Iterate over the direct blocks
    for (int i = 0; i < 12; ++i) {
        // Set the bit in the data bitmap corresponding to the direct block
        if(inode.direct_blocks[i] == 0) {
            continue;
        }
        set_data_block_bitmap(inode.direct_blocks[i], false);
    }

    uint32_t buffer[SYSTEM_POINTER_PER_BLOCK];
    // Handle single indirect pointer
    if (inode.single_indirect != 0) {
        read_block(inode.single_indirect, buffer);
        set_data_block_bitmap(inode.single_indirect, false);
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if(buffer[i] == 0) {
                continue;
            }
            set_data_block_bitmap(buffer[i], false);
        }
    }

    // Handle double indirect pointer
    if (inode.double_indirect != 0) {
        read_block(inode.double_indirect, buffer);
        set_data_block_bitmap(inode.double_indirect, false);
        uint32_t inner_buffer[SYSTEM_POINTER_PER_BLOCK];
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if(buffer[i] == 0) {
                continue;
            }
            set_data_block_bitmap(buffer[i], false);
            read_block(buffer[i], inner_buffer);
            for (int j = 0; j < SYSTEM_POINTER_PER_BLOCK; ++j) {
                if(inner_buffer[j] == 0) {
                    continue;
                }
                set_data_block_bitmap(inner_buffer[j], false);
            }
        }
    }

    // Handle triple indirect pointer
    if (inode.triple_indirect != 0) {
        read_block(inode.triple_indirect, buffer);
        set_data_block_bitmap(inode.triple_indirect, false);
        uint32_t inner_buffer1[SYSTEM_POINTER_PER_BLOCK];
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if(buffer[i] == 0) {
                continue;
            }
            set_data_block_bitmap(buffer[i], false);
            read_block(buffer[i], inner_buffer1);
            uint32_t inner_buffer2[SYSTEM_POINTER_PER_BLOCK];
            for (int j = 0; j < SYSTEM_POINTER_PER_BLOCK; ++j) {
                if(inner_buffer1[j] == 0) {
                    continue;
                }
                set_data_block_bitmap(inner_buffer1[j], false);
                read_block(inner_buffer1[j], inner_buffer2);
                for (int k = 0; k < SYSTEM_POINTER_PER_BLOCK; ++k) {
                    if(inner_buffer2[k] == 0) {
                        continue;
                    }
                    set_data_block_bitmap(inner_buffer2[k], false);
                }
            }
        }
    }

}


void deallocate_data_bitmap_replicate() {
    if(!data_bitmap_combined){
        for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
            delete[] DATA_BITMAPS_REPLICATE[i];
        }
        delete[] DATA_BITMAPS_REPLICATE;
    }
    data_bitmap_combined = true;
    data_bitmap_allocated = false;
}

void set_data_blocks_pointed_in_inodes(){
    data_bitmap_combined = false;
    allocate_data_bitmap_replicate();

    // set data bitmaps of the block groups until data blocks
    for(int i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; i++){
        ext2_block_group_descriptor bgd = BLOCK_GROUP_DESCRIPTORS[i];
        uint32_t set_until = bgd.inode_table + ((SYSTEM_INODE_TABLE_SIZE + SYSTEM_BLOCK_SIZE - 1) / SYSTEM_BLOCK_SIZE);
        uint32_t start_block = i * SUPER_BLOCK->blocks_per_group;
        for(int j = 0; j < set_until - start_block - BOOT_AND_SUPER_BLOCK_OFFSET; j++) {
            set_data_block_bitmap(start_block + j, true);
        }
    }

    // Traverse inodes and set pointed data blocks
    for(int i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; i++) {
        read_inode_tables_by_group_index_no_print(i);
        for(int j = 0; j < SYSTEM_INODE_PER_GROUP; j++) {
            set_data_blocks_of_inode_in_bitmap(LAST_READ_INODE_TABLE[j]);
        }
    }
}

bool is_file_content_block(char* block_content) {
    for(int i = 0; i < 32; i++){
        if(IDENTIFIER[i] != block_content[i])
            return false;
    }
    return true;
}


/**
 * Checks if a block is pointer block or not
 * @return
 */
bool is_pointer_block(char* block_content){
    if(is_directory_block(block_content)){
        return false;
    }
    if(is_file_content_block(block_content)){
        return false;
    }

    return true;
}


/**
 * Read quarter of a block and if not all 0 set in data-bitmap
 */
void set_data_blocks_by_traverse() {
    data_bitmap_combined = false;

    // FIRST ALLOCATE BITMAP REPLICATE allocate_data_bitmap_replicate();

    uint32_t check_size = SYSTEM_BLOCK_SIZE;
    char check_buffer[SYSTEM_BLOCK_SIZE];

    uint32_t total_blocks = SUPER_BLOCK->block_count;

    for (uint32_t block_number = 0; block_number < total_blocks; ++block_number) {
        // Seek to the start of the block
        FILE_IMAGE.seekg(block_number * SYSTEM_BLOCK_SIZE);

        // Read the first 256 bytes of the block
        FILE_IMAGE.read(check_buffer, SYSTEM_BLOCK_SIZE);

        // Check if any of the first quarter are non-zero
        bool has_data = false;
        for (int i = 0; i < check_size; ++i) {
            if (check_buffer[i] != 0) {
                has_data = true;
                break;
            }
        }

        // If there is data, set the corresponding bit in the bitmap
        if (has_data) {
            if(is_set_block_bitmap(block_number, false)){
                continue;
            }

            // THESE BLOCKS WERE NOT SET IN INODE TRAVERSAL

            // DIRECTORY FIRST ENTRY POINTER FIX
            if(is_directory_block(check_buffer)){
                if(!set_first_directory_pointer_of_inode(check_buffer, block_number)) {
                    // IF not first directory entry block, add here because they are net recovered
                    add_orphan_block_group_descriptor(block_number, DIRECTORY, check_buffer);
                }
                set_data_block_bitmap(block_number, false);
                continue;
            }

            if(!is_file_content_block(check_buffer)){
                add_orphan_block_group_descriptor(block_number, POINTER, check_buffer);
                // NOT DIRECTORY NOT FILE CONTENT -----> POINTER BLOCK
                // If pointer to pointers is missing, it is necessary to set all blocks that is pointed in this block
                uint32_t * pointers = reinterpret_cast<uint32_t *>(check_buffer);
                for(int i = 0; i < SYSTEM_POINTER_PER_BLOCK; i++) {
                    set_data_block_bitmap(pointers[i], false);
                }
            }
            set_data_block_bitmap(block_number, false);
        }
    }
}

void set_redundant_data_bitmap_bits() {
    // Set the redundant bits in data bitmap of the last block group
    uint32_t theoretical_block_count = SYSTEM_BLOCK_GROUP_COUNT * SUPER_BLOCK->blocks_per_group; // 2048
    uint32_t actual_block_count = SUPER_BLOCK->block_count - BOOT_AND_SUPER_BLOCK_OFFSET;
    if(actual_block_count != theoretical_block_count){
        uint32_t blocks_of_last_group = actual_block_count % SUPER_BLOCK->blocks_per_group;
        uint32_t set_from_end = SUPER_BLOCK->blocks_per_group - blocks_of_last_group;
        if (set_from_end > 0) {
            uint32_t last_group = SYSTEM_BLOCK_GROUP_COUNT - 1;
            for (uint32_t i = 0; i < set_from_end; ++i) {
                uint32_t block_index = SUPER_BLOCK->blocks_per_group - 1 - i;
                uint32_t byte_index = block_index / 8;
                uint32_t bit_index = block_index % 8;
                BLOCK_GROUP_DATA_BITMAPS[last_group][byte_index] |= (1 << bit_index);
            }
        }
    }
}

/**
 * USE AFTER set_data_blocks_pointed_in_inodes, if data of replicate bitmap is true to be data_bitmap_combined
 * Combine replicate bitmap after set_data_blocks_pointed_in_inodes function with actual bitmap
 */
void combine_data_bitmaps(){
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        for (uint32_t j = 0; j < SYSTEM_DATA_BLOCK_BITMAP_SIZE; ++j) {
            BLOCK_GROUP_DATA_BITMAPS[i][j] |= DATA_BITMAPS_REPLICATE[i][j];
        }
    }
    deallocate_data_bitmap_replicate();
}

