#include <cstring>
#include "directory_table_handler.h"
#include "global_vars.h"
#include "write_handler.h"
#include "inode_bitmap_handler.h"
#include "data_bitmap_handler.h"
#include "iostream"

char* file_content_block_square;
size_t read_size = 0;
uint32_t * current_block_pointer_block;
uint32_t pointer_per_block = 0;
bool is_single_indirect_read = false;
ext2_inode INODE;
uint32_t INODE_NUMBER;

void set_inode_read_direct_blocks_no_print(ext2_inode* inode, uint32_t inode_number) {
    read_size = SYSTEM_BLOCK_SIZE* SYSTEM_BLOCK_SIZE;
    if((inode->mode & 0xf000) != EXT2_I_DTYPE) {
        return;
    }
    INODE_NUMBER = inode_number;
    INODE = *inode;

    delete [] file_content_block_square;
    file_content_block_square = new char[SYSTEM_BLOCK_SIZE * SYSTEM_BLOCK_SIZE];
    std::memset(file_content_block_square, 0, SYSTEM_BLOCK_SIZE * SYSTEM_BLOCK_SIZE);
    pointer_per_block = SYSTEM_BLOCK_SIZE/ sizeof(uint32_t);
    delete [] current_block_pointer_block;
    current_block_pointer_block = new uint32_t[pointer_per_block];
    std::memset(current_block_pointer_block, 0, pointer_per_block * sizeof(uint32_t));
    for(int i = 0; i < 12; i++) {
        if(inode->direct_blocks[i] == 0) {
            break;
        }
        FILE_IMAGE.seekg(inode->direct_blocks[i] * SYSTEM_BLOCK_SIZE);
        FILE_IMAGE.read(file_content_block_square + (i * SYSTEM_BLOCK_SIZE), SYSTEM_BLOCK_SIZE);
    }
    read_directory_entries();
}


void read_directory_entries() {
    read_size = SYSTEM_BLOCK_SIZE * SYSTEM_BLOCK_SIZE;
    size_t offset = 0;
    while (offset < read_size) {
        ext2_dir_entry* entry = reinterpret_cast<ext2_dir_entry*>(file_content_block_square + offset);
        if(!entry->length) {
            break;
        }

        // Move to the next entry
        offset += entry->length;
        if(!is_single_indirect_read && INODE.single_indirect != 0) {
            FILE_IMAGE.seekg(INODE.single_indirect * SYSTEM_BLOCK_SIZE);
            FILE_IMAGE.read(reinterpret_cast<char *>(current_block_pointer_block), SYSTEM_BLOCK_SIZE);
            for(int i = 0; i < pointer_per_block; i++) {
                FILE_IMAGE.seekg(current_block_pointer_block[i] * SYSTEM_BLOCK_SIZE);
                FILE_IMAGE.read(file_content_block_square + (i * SYSTEM_BLOCK_SIZE), SYSTEM_BLOCK_SIZE);
            }
            is_single_indirect_read = true;
            offset = 0;
        }
    }
}

void read_indirect() {
    FILE_IMAGE.seekg(INODE.single_indirect * SYSTEM_BLOCK_SIZE);
    FILE_IMAGE.read(reinterpret_cast<char *>(current_block_pointer_block), SYSTEM_BLOCK_SIZE);
    for(int i = 0; i < pointer_per_block; i++) {
        FILE_IMAGE.seekg(current_block_pointer_block[i] * SYSTEM_BLOCK_SIZE);
        FILE_IMAGE.read(file_content_block_square + (i * SYSTEM_BLOCK_SIZE), SYSTEM_BLOCK_SIZE);
    }
    is_single_indirect_read = true;
}

void print_all_directory_structure_recursively(int hyphen_count) {
    read_size = SYSTEM_BLOCK_SIZE * SYSTEM_BLOCK_SIZE;
    uint32_t entry_length;
    uint32_t entry_inode;
    uint32_t entry_name_length;
    size_t offset = 0;
    int read_file_count = 0;
    while (offset < read_size) {
        ext2_dir_entry* entry = reinterpret_cast<ext2_dir_entry*>(file_content_block_square + offset);
        entry_length = entry->length;
        entry_inode = entry->inode;
        entry_name_length = entry->name_length;
        if(!entry_length || entry_inode == 0 || entry_length >= SYSTEM_BLOCK_SIZE || entry_name_length > 255 || entry_inode > (SYSTEM_INODE_PER_GROUP * SYSTEM_BLOCK_GROUP_COUNT)) {
            break;
        }

        if(read_file_count > 1 && entry->inode) {
            for (int i = 0; i <  hyphen_count; ++i) {
                std::cout << "-";
            }
            std::cout << " ";
            for (uint8_t i = 0; i < entry->name_length; ++i) {
                std::cout << entry->name[i];
            }

            uint32_t store_inode_number = INODE_NUMBER;
            ext2_inode store_inode = INODE;

            if(entry_inode != 0 && (INODE_NUMBER - 1) / SYSTEM_INODE_PER_GROUP !=  (entry_inode - 1) / SYSTEM_INODE_PER_GROUP) {
                read_inode_tables_by_group_index_no_print((entry_inode - 1) / SYSTEM_INODE_PER_GROUP);
            }
            bool is_entry_directory = (LAST_READ_INODE_TABLE[(entry_inode - 1) % SYSTEM_INODE_PER_GROUP].mode & 0xf000) == EXT2_I_DTYPE;
            set_inode_read_direct_blocks_no_print(&LAST_READ_INODE_TABLE[(entry_inode - 1) % SYSTEM_INODE_PER_GROUP], (entry_inode - 1) % SYSTEM_INODE_PER_GROUP);
            if(is_entry_directory){
                std::cout << "/";
                std::cout << std::endl;
                print_all_directory_structure_recursively(hyphen_count + 1);
            } else{
                std::cout << std::endl;
            }
            INODE_NUMBER = store_inode_number;
            INODE = store_inode;
            if(is_single_indirect_read) {
                read_indirect();
            }
            else{
                set_inode_read_direct_blocks_no_print(&INODE, INODE_NUMBER);
            }
        }

        // Move to the next entry
        offset += entry_length;
        if(!is_single_indirect_read && INODE.single_indirect != 0) {
            read_indirect();
            offset = 0;
        }
        read_file_count++;
    }
}

void print_directory_structure() {
    if((ROOT_INODE.mode & 0xf000) != EXT2_I_DTYPE){
        return;
    }
    std::cout << "- " << "root/" << std::endl;

    // To suppress segmentation fault
    set_inode_read_direct_blocks_no_print(&ROOT_INODE, 2);
    print_all_directory_structure_recursively(2);
}

/**
 * Checks if the block is the directory block or not
 * @param block_content
 * @return
 */
bool is_directory_block(char* block_content) {
    if(is_file_content_block(block_content)){
        return false;
    }
    read_size = SYSTEM_BLOCK_SIZE;
    int offset = 0;
    while (offset < read_size) {
        ext2_dir_entry* entry = reinterpret_cast<ext2_dir_entry*>(block_content + offset);
        if(entry->inode == 0){
            if(entry->length == 0){
                return false;
            }
            offset+= entry->length;
            continue;
        }

        if(entry->inode > SYSTEM_INODE_PER_GROUP * SYSTEM_BLOCK_GROUP_COUNT){
            return false;
        }

        if(entry->length % 4 != 0 || entry->length < 8){
            return false;
        }

        if(entry->name_length > 255 || entry->name_length == 0) {
            return false;
        }

        // Move to the next entry
        offset += entry->length;
    }

    return true;
}

bool is_directory_first_entry(ext2_inode inode, uint32_t inode_number, uint32_t* get_parent_dir) {
    if(inode.direct_blocks[0] == 0) {
        return false;
    }

    char * content = new char[SYSTEM_BLOCK_SIZE];
    size_t read_size = SYSTEM_BLOCK_SIZE;
    size_t offset = 0;
    FILE_IMAGE.seekg(inode.direct_blocks[0] * SYSTEM_BLOCK_SIZE);
    FILE_IMAGE.read(content, SYSTEM_BLOCK_SIZE);
    int read_file_count = 0;

    while (offset < read_size && read_file_count < 2) {
        ext2_dir_entry* entry = reinterpret_cast<ext2_dir_entry*>(content + offset);
        // MUST READ ".", pointed to the current directory
        bool isDir = true;
        if(read_file_count == 0) {

            isDir &= (entry->inode == inode_number || inode.link_count > 1);
            isDir &= entry->length == 12; // HARDCODED
            isDir &= entry->name_length == 1; // HARDCODED
            isDir &= entry->name[0] == '.'; // HARDCODED
            if(!isDir){
                delete [] content;
                *get_parent_dir = 0;
                return false;
            }
        }

        // MUST READ ".." pointed to parent directory
        if(read_file_count == 1) {
            // NO INODE NUMBER CHECK
            isDir &= entry->length >= 12 || entry->length == (SYSTEM_BLOCK_SIZE - 12); // HARDCODED
            isDir &= entry->name_length == 2; // HARDCODED
            isDir &= entry->name[0] == '.'; // HARDCODED
            isDir &= entry->name[1] == '.'; // HARDCODED
            *get_parent_dir = entry->inode;
            if(!isDir){
                delete [] content;
                *get_parent_dir = 0;
                return false;
            }
        }

        // Move to the next entry
        offset += entry->length;
        read_file_count++;
    }
    delete [] content;
    return true;
}

bool set_first_directory_pointer_of_inode(char * block_content, uint32_t block_number){
    ext2_dir_entry* entry = reinterpret_cast<ext2_dir_entry*>(block_content);
    // MUST READ ".", pointed to the current directory
    bool is_first_dir_entry = true;
    is_first_dir_entry &= entry->length == 12; // HARDCODED
    is_first_dir_entry &= entry->name_length == 1; // HARDCODED
    is_first_dir_entry &= entry->name[0] == '.'; // HARDCODED
    if(!is_first_dir_entry){
        return false;
    }

    uint32_t directory_inode_number = entry->inode;
    if (directory_inode_number == 0) {
        return true;
    }

    block_number += (BOOT_AND_SUPER_BLOCK_OFFSET);
    uint32_t inode_index = directory_inode_number - 1;
    uint32_t block_group = inode_index / SUPER_BLOCK->inodes_per_group;
    uint32_t index_in_group = inode_index % SUPER_BLOCK->inodes_per_group;
    read_inode_tables_by_group_index_no_print(block_group);
    if(LAST_READ_INODE_TABLE[index_in_group].direct_blocks[0] == 0){
        LAST_READ_INODE_TABLE[index_in_group].direct_blocks[0] = (block_number - BOOT_AND_SUPER_BLOCK_OFFSET); // MAY CAUSE ERROR
        if(single_inode_consistency_check(LAST_READ_INODE_TABLE[index_in_group], directory_inode_number, false)){
            write_one_inode(directory_inode_number, LAST_READ_INODE_TABLE[index_in_group]);
        }
    }
    return true;
}

uint32_t get_parent_directory_inode_number_by_directory_block(char* block_content) {

    size_t offset = 0;
    uint32_t block_size = SYSTEM_BLOCK_SIZE;
    while (offset < block_size) {
        ext2_dir_entry* entry = reinterpret_cast<ext2_dir_entry*>(block_content + offset);
        if(!entry->length) {
            break;
        }
        if(entry->inode != 0) {

            uint32_t group_index = (entry->inode - 1) / SYSTEM_INODE_PER_GROUP;
            uint32_t inode_index = (entry->inode -1) % SYSTEM_INODE_PER_GROUP;
            read_inode_tables_by_group_index_no_print(group_index);

            if((LAST_READ_INODE_TABLE[inode_index].mode & 0xf000) != EXT2_I_DTYPE){
                uint32_t parent_dir_inode_number;
                is_directory_first_entry(LAST_READ_INODE_TABLE[inode_index], entry->inode, &parent_dir_inode_number);
            }
        }
        // Move to the next entry
        offset += entry->length;
    }

    return 0;
}

void deallocate_file_content() {
    delete [] file_content_block_square;
    delete [] current_block_pointer_block;
}

