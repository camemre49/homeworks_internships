#include <cstring>
#include <algorithm>
#include "residual_file_content_handler.h"
#include "ext2fs.h"
#include "global_vars.h"
#include "pointer-recovery-handler.h"
#include "vector"
#include "write_handler.h"

std::vector<deficient_inode> deficient_residual_file_inodes;
std::vector<block_descriptor> orphan_residual_block_descriptors;
char** ORPHAN_BLOCKS_NOT_POINTED_BY_INODES;

uint32_t get_min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}

// Enumeration for file modes
enum Ext2FileMode {
    SOCKET = 0xC000,
    SYMBOLIC_LINK = 0xA000,
    REGULAR_FILE = EXT2_I_FTYPE,
    BLOCK_DEVICE = 0x6000,
    DIR = EXT2_I_DTYPE,
    CHARACTER_DEVICE = 0x2000,
    FIFO = 0x1000,
    UNKNOWN = 0x0000 // Added to handle unknown types
};

// Function to get the mode as an enum
enum Ext2FileMode get_mode(uint16_t mode) {
    switch (mode & 0xF000) { // Masking with 0xF000 to get the type
        case SOCKET: return SOCKET;
        case SYMBOLIC_LINK: return SYMBOLIC_LINK;
        case REGULAR_FILE: return REGULAR_FILE;
        case BLOCK_DEVICE: return BLOCK_DEVICE;
        case DIR: return DIR;
        case CHARACTER_DEVICE: return CHARACTER_DEVICE;
        case FIFO: return FIFO;
        default: return UNKNOWN; // Handle unknown types
    }
}

bool is_set_block_bitmap_residual(uint32_t block_number, bool no_fix = false) {
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
    return (ORPHAN_BLOCKS_NOT_POINTED_BY_INODES[block_group][bitmap_byte] & (1 << bitmap_bit)) != 0;
}

void read_block_residual(uint32_t block_ptr, uint32_t* buffer) {
    if (!FILE_IMAGE.is_open()) {
        return;
    }

    uint32_t byte_offset = block_ptr * SYSTEM_BLOCK_SIZE;
    FILE_IMAGE.seekg(byte_offset);
    FILE_IMAGE.read(reinterpret_cast<char*>(buffer), SYSTEM_BLOCK_SIZE);
}

void set_data_block_bitmap_residual(uint32_t block_number, bool no_fix) {
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
    ORPHAN_BLOCKS_NOT_POINTED_BY_INODES[block_group][bitmap_byte] |= (1 << bitmap_bit);
}

void set_data_blocks_of_inode_in_bitmap_residual(ext2_inode inode) {
    // Iterate over the direct blocks
    for (int i = 0; i < 12; ++i) {
        // Set the bit in the data bitmap corresponding to the direct block
        if(inode.direct_blocks[i] == 0) {
            continue;
        }
        set_data_block_bitmap_residual(inode.direct_blocks[i], false);
    }

    uint32_t buffer[SYSTEM_POINTER_PER_BLOCK];
    // Handle single indirect pointer
    if (inode.single_indirect != 0) {
        read_block_residual(inode.single_indirect, buffer);
        set_data_block_bitmap_residual(inode.single_indirect, false);
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if(buffer[i] == 0) {
                continue;
            }
            set_data_block_bitmap_residual(buffer[i], false);
        }
    }

    // Handle double indirect pointer
    if (inode.double_indirect != 0) {
        read_block_residual(inode.double_indirect, buffer);
        set_data_block_bitmap_residual(inode.double_indirect, false);
        uint32_t inner_buffer[SYSTEM_POINTER_PER_BLOCK];
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if(buffer[i] == 0) {
                continue;
            }
            set_data_block_bitmap_residual(buffer[i], false);
            read_block_residual(buffer[i], inner_buffer);
            for (int j = 0; j < SYSTEM_POINTER_PER_BLOCK; ++j) {
                if(inner_buffer[j] == 0) {
                    continue;
                }
                set_data_block_bitmap_residual(inner_buffer[j], false);
            }
        }
    }

    // Handle triple indirect pointer
    if (inode.triple_indirect != 0) {
        read_block_residual(inode.triple_indirect, buffer);
        set_data_block_bitmap_residual(inode.triple_indirect, false);
        uint32_t inner_buffer1[SYSTEM_POINTER_PER_BLOCK];
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if(buffer[i] == 0) {
                continue;
            }
            set_data_block_bitmap_residual(buffer[i], false);
            read_block_residual(buffer[i], inner_buffer1);
            uint32_t inner_buffer2[SYSTEM_POINTER_PER_BLOCK];
            for (int j = 0; j < SYSTEM_POINTER_PER_BLOCK; ++j) {
                if(inner_buffer1[j] == 0) {
                    continue;
                }
                set_data_block_bitmap_residual(inner_buffer1[j], false);
                read_block_residual(inner_buffer1[j], inner_buffer2);
                for (int k = 0; k < SYSTEM_POINTER_PER_BLOCK; ++k) {
                    if(inner_buffer2[k] == 0) {
                        continue;
                    }
                    set_data_block_bitmap_residual(inner_buffer2[k], false);
                }
            }
        }
    }

}

void allocate_orphan_blocks() {
    ORPHAN_BLOCKS_NOT_POINTED_BY_INODES = new char *[SYSTEM_BLOCK_GROUP_COUNT];
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        ORPHAN_BLOCKS_NOT_POINTED_BY_INODES[i] = new char[SYSTEM_DATA_BLOCK_BITMAP_SIZE];
        std::memset(ORPHAN_BLOCKS_NOT_POINTED_BY_INODES[i], 0, SYSTEM_DATA_BLOCK_BITMAP_SIZE); // Initialize to zero
    }
}

void deallocate_orphan_blocks() {
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        delete [] ORPHAN_BLOCKS_NOT_POINTED_BY_INODES[i];
    }

    delete [] ORPHAN_BLOCKS_NOT_POINTED_BY_INODES;
}

/**
 * Read quarter of a block and if not all 0 set in data-bitmap
 */
void set_data_blocks_by_traverse_residual() {
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
            if(is_set_block_bitmap_residual(block_number, false)){
                continue;
            }

            uint32_t last_byte = 0;
            for(int i = 0; i < SYSTEM_BLOCK_SIZE; i++){
                if(check_buffer[i]){
                    last_byte = i + 1;
                }
            }

            block_descriptor bd = {};
            bd.block_number = block_number;
            bd.block_content_size = last_byte * sizeof(char);
            orphan_residual_block_descriptors.push_back(bd);
        }
    }
}

void sort_direct_pointers_residual(ext2_inode * inode){
    const uint32_t DIRECT_POINTER_COUNT = 12;
    uint32_t direct_pointers[DIRECT_POINTER_COUNT];

    // Extract the direct pointers
    for (int i = 0; i < DIRECT_POINTER_COUNT; ++i) {
        direct_pointers[i] = inode->direct_blocks[i];
    }

    // Sort the non-zero elements
    std::sort(direct_pointers, direct_pointers + DIRECT_POINTER_COUNT, [](uint32_t a, uint32_t b) {
        if (a == 0) return false;
        if (b == 0) return true;
        return a < b;
    });

    // Update the inode's direct pointers with the sorted pointers
    for (int i = 0; i < DIRECT_POINTER_COUNT; ++i) {
        inode->direct_blocks[i] = direct_pointers[i];
    }
}

// ASSUME ONLY ONE DEFICIENT RESIDUAL IS LEFT
void insert_residual_pointers() {
    for(auto& di : deficient_residual_file_inodes) {
        for(auto & orbd : orphan_residual_block_descriptors){
            ext2_inode* inode = &di.inode;
            uint32_t inode_number = di.inode_number;
            int index = 0;
            for (short missing_pointer_indexes : di.missing_pointer_indexes) {
                if (index < orphan_residual_block_descriptors.size()) {
                    if(orphan_residual_block_descriptors[index].block_number == 0) {
                        continue;
                    }
                    if(missing_pointer_indexes == 100) {
                        break;
                    }
                    inode->direct_blocks[missing_pointer_indexes] = orphan_residual_block_descriptors[index].block_number;
                    index++;
                }
            }
            sort_direct_pointers_residual(inode);
            write_one_inode(inode_number, *inode);
        }
    }
}


void recover_residual_file_content_blocks(){
    allocate_orphan_blocks();

    for(int i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; i++) {
        read_inode_tables_by_group_index_no_print(i);
        for(int j = 0; j < SYSTEM_INODE_PER_GROUP; j++) {
            ext2_inode * inode = LAST_READ_INODE_TABLE + j;
            uint32_t inode_number = i * SYSTEM_INODE_PER_GROUP + j + 1;
            deficient_inode di = {};
            int missing_pointer_index = 0;
            di.missing_pointer_indexes[0] = 100;
            bool is_deficient = false;

            uint32_t block_count = (inode->size + SYSTEM_BLOCK_SIZE - 1) / SYSTEM_BLOCK_SIZE;
            // Check direct pointers
            for (short direct_index = 0; direct_index < get_min(block_count, 12); ++direct_index) {
                if(inode->direct_blocks[direct_index] == 0){
                    di.missing_pointer_indexes[missing_pointer_index] = direct_index;
                    if(direct_index != 11){
                        di.missing_pointer_indexes[missing_pointer_index + 1] = 100;
                    }
                    missing_pointer_index++;
                    is_deficient = true;
                }
            }

            // Check single indirect pointer, if it is not zero that means it is an inode like
            // direct<12> = 0  and (indirect = 1 or double_indirect = 1 or triple_indirect = 1)
            if (block_count > 12) {
                if (inode->single_indirect == 0) {
                    continue;
                }
            }

            // Check double indirect pointer, if it is not zero that means it is an inode like
            // direct<12> = 0  and (indirect = 1 or double_indirect = 1 or triple_indirect = 1)
            if (block_count > 12 + SYSTEM_POINTER_PER_BLOCK) {
                if (inode->double_indirect == 0) {
                    continue;
                }
            }

            // Check triple indirect pointer, if it is not zero that means it is an inode like
            // direct<12> = 0  and (indirect = 1 or double_indirect = 1 or triple_indirect = 1)
            if (block_count > 12 + SYSTEM_POINTER_PER_BLOCK + SYSTEM_POINTER_PER_BLOCK * SYSTEM_POINTER_PER_BLOCK) {
                if (inode->triple_indirect == 0) {
                    continue;
                }
            }

            if(is_deficient){
                di.inode = *inode;
                di.inode_number = inode_number;
                deficient_residual_file_inodes.push_back(di);
            }
        }
    }

    // set data bitmaps of the block groups until data blocks
    for(int i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; i++){
        ext2_block_group_descriptor bgd = BLOCK_GROUP_DESCRIPTORS[i];
        uint32_t set_until = bgd.inode_table + ((SYSTEM_INODE_TABLE_SIZE + SYSTEM_BLOCK_SIZE - 1) / SYSTEM_BLOCK_SIZE);
        uint32_t start_block = i * SUPER_BLOCK->blocks_per_group;
        for(int j = 0; j < set_until - start_block - BOOT_AND_SUPER_BLOCK_OFFSET; j++) {
            uint32_t block_number = start_block + j;
            if(block_number > SUPER_BLOCK->block_count){
                return;
            }
            uint32_t block_group = block_number  / SUPER_BLOCK->blocks_per_group;
            uint32_t index_in_block_group = block_number % SUPER_BLOCK->blocks_per_group;
            uint32_t bitmap_byte = index_in_block_group / 8;
            uint32_t bitmap_bit = index_in_block_group % 8;
            ORPHAN_BLOCKS_NOT_POINTED_BY_INODES[block_group][bitmap_byte] |= (1 << bitmap_bit);
        }
    }

    for(int i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; i++) {
        read_inode_tables_by_group_index_no_print(i);
        for(int j = 0; j < SYSTEM_INODE_PER_GROUP; j++) {
            set_data_blocks_of_inode_in_bitmap_residual(LAST_READ_INODE_TABLE[j]);
        }
    }

    set_data_blocks_by_traverse_residual();
    insert_residual_pointers();

    deallocate_orphan_blocks();
}