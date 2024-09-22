#include <cstring>
#include "inode_bitmap_handler.h"
#include "global_vars.h"
#include "directory_table_handler.h"

char** INODE_BITMAPS_REPLICATE;
bool inode_bitmap_combined = false;

// Enumeration for file modes
enum Ext2FileMode {
    SOCKET = 0xC000,
    SYMBOLIC_LINK = 0xA000,
    REGULAR_FILE = EXT2_I_FTYPE,
    BLOCK_DEVICE = 0x6000,
    DIRECTORY = EXT2_I_DTYPE,
    CHARACTER_DEVICE = 0x2000,
    FIFO = 0x1000,
    UNKNOWN = 0x0000 // Added to handle unknown types
};

// Function to get the mode as an enum
enum Ext2FileMode get_mode_enum(uint16_t mode) {
    switch (mode & 0xF000) { // Masking with 0xF000 to get the type
        case SOCKET: return SOCKET;
        case SYMBOLIC_LINK: return SYMBOLIC_LINK;
        case REGULAR_FILE: return REGULAR_FILE;
        case BLOCK_DEVICE: return BLOCK_DEVICE;
        case DIRECTORY: return DIRECTORY;
        case CHARACTER_DEVICE: return CHARACTER_DEVICE;
        case FIFO: return FIFO;
        default: return UNKNOWN; // Handle unknown types
    }
}

void allocate_inode_bitmap() {
    INODE_BITMAPS_REPLICATE = new char *[SYSTEM_BLOCK_GROUP_COUNT];
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        INODE_BITMAPS_REPLICATE[i] = new char[SYSTEM_INODE_BITMAP_SIZE];
        std::memset(INODE_BITMAPS_REPLICATE[i], 0, SYSTEM_INODE_BITMAP_SIZE); // Initialize to zero
    }
}

/**
 * Check if there is any conflict inside information of an inode
 * Save the content in a replicate inode bitmap
 */
void inode_consistency_check(bool assume_pointers_intact){
    inode_bitmap_combined = false;
    allocate_inode_bitmap();
    for(int i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; i++) {
        read_inode_tables_by_group_index_no_print(i);
        for(int j = 0; j < SYSTEM_INODE_PER_GROUP; j++) {
            if(i == 0 && j < 10) {
                int inode_byte = j / 8;
                int inode_bit = j % 8;
                INODE_BITMAPS_REPLICATE[i][inode_byte] |= (1 << inode_bit);
                continue;
            }
            if(single_inode_consistency_check(LAST_READ_INODE_TABLE[j], i * SYSTEM_INODE_PER_GROUP + j + 1, assume_pointers_intact)) {
                int inode_byte = j / 8;
                int inode_bit = j % 8;
                INODE_BITMAPS_REPLICATE[i][inode_byte] |= (1 << inode_bit);
            }
        }
    }
}


uint32_t direct_pointer_check(ext2_inode &inode);
bool single_inode_consistency_check(ext2_inode inode, uint32_t inode_number, bool assume_pointers_intact){
    // Just a placeholder for function is_directory_first_entry
    uint32_t null;
    // Mode validity
    if(get_mode_enum(inode.mode) == UNKNOWN){
        return false;
    }
    else if(get_mode_enum(inode.mode) == DIRECTORY && !is_directory_first_entry(inode, inode_number, &null)){
        return false;
    }

    // Access, creation, modification time validity
    if(inode.access_time == 0 || inode.creation_time == 0 || inode.modification_time == 0) {
        return false;
    }
    // There are some inodes with -1 access time compared to creation time
    if(inode.access_time < (inode.creation_time - 5) || inode.modification_time < (inode.creation_time - 5)){
        return false;
    }

    // Deletion time validity
    if(inode.deletion_time != 0) {
        // File is deleted
        return false;
    }

    if(assume_pointers_intact){

        // Size and pointer consistency
        uint32_t pointer_count = (inode.size + (SYSTEM_BLOCK_SIZE - 1)) / SYSTEM_BLOCK_SIZE;
        if(pointer_count <= 12){
            if(pointer_count != direct_pointer_check(inode)){
                return false;
            }
        }
        // If indirect is the last pointer
        else if (pointer_count < 12 + (SYSTEM_BLOCK_SIZE / 4)) {
            if(direct_pointer_check(inode) != 12){
                return false;
            }
            if(inode.single_indirect == 0){
                return false;
            }
        }
        // If double pointer is the last pointer
        else if (pointer_count < 12 + (SYSTEM_BLOCK_SIZE / 4) * (SYSTEM_BLOCK_SIZE / 4)){
            if(direct_pointer_check(inode) != 12){
                return false;
            }
            if(inode.single_indirect == 0){
                return false;
            }
            if(inode.double_indirect == 0){
                return false;
            }
        }
        // If triple pointer is the last pointer
        else if (pointer_count < 12 + (SYSTEM_BLOCK_SIZE / 4) * (SYSTEM_BLOCK_SIZE / 4) * (SYSTEM_BLOCK_SIZE / 4)){
            if(direct_pointer_check(inode) != 12){
                return false;
            }
            if(inode.single_indirect == 0){
                return false;
            }
            if(inode.double_indirect == 0){
                return false;
            }
            if(inode.triple_indirect == 0){
                return false;
            }
        }
        else {
            return false;
        }
    }
    return true;
}

uint32_t direct_pointer_check(ext2_inode &inode){
    int index = 0;
    for(; index < 12; index++){
        if(!inode.direct_blocks[index]){
            return index;
        }
    }
    return index;
}

/**
 * USE AFTER consistency_check, if data of replicate bitmap is true to be inode_bitmap_combined
 * Combine replicate bitmap after inode_consistency_check function with actual bitmap
 */
void combine_inode_bitmaps(){
    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        for (uint32_t j = 0; j < SYSTEM_INODE_BITMAP_SIZE; ++j) {
            BLOCK_GROUP_INODE_BITMAPS[i][j] |= INODE_BITMAPS_REPLICATE[i][j];
        }
    }
    deallocate_inode_bitmap_replicate();
}

void deallocate_inode_bitmap_replicate() {
    if(!inode_bitmap_combined){
        for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
            delete[] INODE_BITMAPS_REPLICATE[i];
        }
        delete[] INODE_BITMAPS_REPLICATE;
    }
    inode_bitmap_combined = true;
}


