#include "global_vars.h"
#include "fstream"


void write_bitmaps(const char* image_path) {
    // Open the file in binary mode for reading and writing
    std::fstream file_image_out(image_path, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_image_out.is_open()) {
        return;
    }

    for (uint32_t i = 0; i < SYSTEM_BLOCK_GROUP_COUNT; ++i) {
        // Write data block bitmap
        file_image_out.seekp(BLOCK_GROUP_DESCRIPTORS[i].block_bitmap * SYSTEM_BLOCK_SIZE);
        if (!file_image_out.write(BLOCK_GROUP_DATA_BITMAPS[i], SYSTEM_DATA_BLOCK_BITMAP_SIZE)) {
            return;
        }

        // Write inode bitmap
        file_image_out.seekp(BLOCK_GROUP_DESCRIPTORS[i].inode_bitmap * SYSTEM_BLOCK_SIZE);
        if (!file_image_out.write(BLOCK_GROUP_INODE_BITMAPS[i], SYSTEM_INODE_BITMAP_SIZE)) {
            return;
        }
    }
    file_image_out.close();
}

void write_one_inode(uint32_t inode_number, ext2_inode &inode){
    uint32_t inode_index = inode_number - 1;
    uint32_t block_group = inode_index / SUPER_BLOCK->inodes_per_group;
    uint32_t index_in_group = inode_index % SUPER_BLOCK->inodes_per_group;
    uint32_t inode_table_start = BLOCK_GROUP_DESCRIPTORS[block_group].inode_table * SYSTEM_BLOCK_SIZE;

    // Calculate the byte offset of the specific inode within the inode table
    uint32_t inode_offset = inode_table_start + (index_in_group * SUPER_BLOCK->inode_size);
    // Open the file in binary mode for reading and writing
    std::fstream file_image_out(IMAGE_PATH, std::ios::in | std::ios::out | std::ios::binary);
    // Seek to the correct position in the file
    file_image_out.seekp(inode_offset);

    // Write the inode back to the disk
    if (!file_image_out.write(reinterpret_cast<char*>(&inode), SUPER_BLOCK->inode_size)) {
    }

}