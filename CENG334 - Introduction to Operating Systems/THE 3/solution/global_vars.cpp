#include "global_vars.h"
#include "ext2fs.h"
#include <cmath>

// PROPERTIES OF THE SYSTEM CURRENTLY HANDLING
std::ifstream FILE_IMAGE;
uint8_t* IDENTIFIER;
char* IMAGE_PATH;
ext2_super_block *SUPER_BLOCK = nullptr;
uint32_t SYSTEM_BLOCK_SIZE; // Block size
uint32_t SYSTEM_INODE_PER_GROUP; // Inode count per group
uint32_t SYSTEM_INODE_TABLE_SIZE; // Inode tables size per group
uint32_t SYSTEM_INODE_BITMAP_SIZE; // Inode bitmap size in bytes
uint32_t SYSTEM_DATA_BLOCK_BITMAP_SIZE; // Data blocks bitmap size in bytes
uint32_t SYSTEM_BLOCK_GROUP_SIZE; // Size of a block group
uint32_t SYSTEM_BLOCK_GROUP_COUNT; // Group count in fs
uint32_t SYSTEM_POINTER_PER_BLOCK; // Pointer per block
uint32_t BOOT_AND_SUPER_BLOCK_OFFSET;
ext2_block_group_descriptor *BLOCK_GROUP_DESCRIPTORS;
ext2_inode *LAST_READ_INODE_TABLE;
ext2_inode ROOT_INODE;
char** BLOCK_GROUP_INODE_BITMAPS;
char** BLOCK_GROUP_DATA_BITMAPS;

void parse_super_block() {
    SYSTEM_BLOCK_SIZE = EXT2_BOOT_BLOCK_SIZE * std::pow(2, SUPER_BLOCK->log_block_size);
    SYSTEM_INODE_PER_GROUP = SUPER_BLOCK->inodes_per_group;
    SYSTEM_INODE_TABLE_SIZE = SYSTEM_INODE_PER_GROUP * SUPER_BLOCK->inode_size;
    SYSTEM_INODE_BITMAP_SIZE = SYSTEM_INODE_PER_GROUP / 8;
    SYSTEM_DATA_BLOCK_BITMAP_SIZE = SUPER_BLOCK->blocks_per_group / 8;
    SYSTEM_BLOCK_GROUP_SIZE = SYSTEM_BLOCK_SIZE * SUPER_BLOCK->blocks_per_group;
    SYSTEM_BLOCK_GROUP_COUNT = (SUPER_BLOCK->block_count + SUPER_BLOCK->blocks_per_group - 1) / SUPER_BLOCK->blocks_per_group; // Ceiling of division
    SYSTEM_POINTER_PER_BLOCK = SYSTEM_BLOCK_SIZE / sizeof(uint32_t);
    BLOCK_GROUP_DESCRIPTORS = new ext2_block_group_descriptor[SYSTEM_BLOCK_GROUP_COUNT];
    LAST_READ_INODE_TABLE = new ext2_inode[SYSTEM_INODE_TABLE_SIZE];
    BOOT_AND_SUPER_BLOCK_OFFSET = (((2048  + (SYSTEM_BLOCK_SIZE - 1)) / SYSTEM_BLOCK_SIZE) - 1);
}


void read_file_image(const char *image_path) {
    // Open the ext2 image file
    FILE_IMAGE.open(image_path, std::ios::binary);
    if (!FILE_IMAGE.is_open()) {
        return;
    }
    read_super_block();
    parse_super_block();
    get_group_descriptor_table_by_index(0);
    read_bitmaps();
}

void read_super_block() {
    // Read super block
    FILE_IMAGE.seekg(EXT2_SUPER_BLOCK_POSITION);
    SUPER_BLOCK = new ext2_super_block;
    if (!FILE_IMAGE.read(reinterpret_cast<char *>(SUPER_BLOCK), sizeof(ext2_super_block))) {
    }
}

uint32_t get_group_start_byte_by_index(int group_index) {
    if (group_index > SYSTEM_BLOCK_GROUP_COUNT - 1) {
        return 0;
    }
    uint32_t start_byte = (EXT2_SUPER_BLOCK_POSITION + ((group_index) * SYSTEM_BLOCK_GROUP_SIZE));
    return start_byte;
}

void get_group_descriptor_table_by_index(int group_index) {
    if (group_index > SYSTEM_BLOCK_GROUP_COUNT - 1) {
        return;
    }
    uint32_t start_byte = get_group_start_byte_by_index(group_index);
    uint32_t byte_after_super_block = ((start_byte + EXT2_SUPER_BLOCK_SIZE + SYSTEM_BLOCK_SIZE - 1) / SYSTEM_BLOCK_SIZE) * SYSTEM_BLOCK_SIZE;
    FILE_IMAGE.seekg(byte_after_super_block);
    if (!FILE_IMAGE.read(reinterpret_cast<char *>(BLOCK_GROUP_DESCRIPTORS),sizeof(ext2_block_group_descriptor) * SYSTEM_BLOCK_GROUP_COUNT)) {
    }
}


void allocate_bitmaps() {
    // Allocate memory for inode bitmaps
    BLOCK_GROUP_INODE_BITMAPS = new char *[SYSTEM_BLOCK_GROUP_COUNT];
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        BLOCK_GROUP_INODE_BITMAPS[i] = new char[SYSTEM_INODE_BITMAP_SIZE];
    }

    // Allocate memory for data block bitmaps
    BLOCK_GROUP_DATA_BITMAPS = new char* [SYSTEM_BLOCK_GROUP_COUNT];
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        BLOCK_GROUP_DATA_BITMAPS[i] = new char [SYSTEM_DATA_BLOCK_BITMAP_SIZE];
    }
}

void read_bitmaps() {
    allocate_bitmaps();  // Ensure bitmaps are allocated

    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        // Read data block bitmap
        FILE_IMAGE.seekg(BLOCK_GROUP_DESCRIPTORS[i].block_bitmap * SYSTEM_BLOCK_SIZE);
        if (!FILE_IMAGE.read(BLOCK_GROUP_DATA_BITMAPS[i], SYSTEM_DATA_BLOCK_BITMAP_SIZE)) {
            return;
        }

        // Read inode bitmap
        FILE_IMAGE.seekg(BLOCK_GROUP_DESCRIPTORS[i].inode_bitmap * SYSTEM_BLOCK_SIZE);
        if (!FILE_IMAGE.read(BLOCK_GROUP_INODE_BITMAPS[i], SYSTEM_INODE_BITMAP_SIZE)) {
            return;
        }

    }
}

void deallocate_bitmaps() {
    // Deallocate memory for inode bitmaps
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        delete[] BLOCK_GROUP_INODE_BITMAPS[i];
    }
    delete[] BLOCK_GROUP_INODE_BITMAPS;

    // Deallocate memory for data block bitmaps
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        delete[] BLOCK_GROUP_DATA_BITMAPS[i];
    }
    delete[] BLOCK_GROUP_DATA_BITMAPS;
}

void deallocate_group_descriptors() {
    delete[] BLOCK_GROUP_DESCRIPTORS;
}


void read_inode_tables_by_group_index_no_print(int group_index) {
    char* table_blocks = new char[SYSTEM_INODE_TABLE_SIZE];
    uint32_t inode_table_start = BLOCK_GROUP_DESCRIPTORS[group_index].inode_table * SYSTEM_BLOCK_SIZE;
    FILE_IMAGE.seekg(inode_table_start);
    if (!FILE_IMAGE.read(table_blocks, SYSTEM_INODE_TABLE_SIZE)) {
        delete[] LAST_READ_INODE_TABLE;
        return;
    }

    for (uint32_t j = 0; j < SYSTEM_INODE_PER_GROUP; ++j) {
        LAST_READ_INODE_TABLE[j] = *(reinterpret_cast<ext2_inode*>(table_blocks + (SUPER_BLOCK->inode_size * j)));
        }
    ROOT_INODE = LAST_READ_INODE_TABLE[1];

    delete [] table_blocks;
}


