#include <algorithm>
#include "pointer-recovery-handler.h"
#include "write_handler.h"

using namespace std;

std::vector<block_descriptor> orphan_pointer_block_group_descriptors;
std::vector<block_descriptor> orphan_direct_directory_block_group_descriptors;
std::vector<deficient_inode> deficient_directory_inodes;
std::vector<deficient_inode> deficient_file_inodes;

uint32_t min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}


uint32_t calculate_block_size(char* block_content) {
    uint32_t last_byte = 0;
    for(int i = 0; i < SYSTEM_BLOCK_SIZE; i++){
        if(block_content[i]){
            last_byte = i + 1;
        }
    }
    return last_byte * sizeof(char);
}

void read_block(uint32_t block_number, char* buffer) {
    FILE_IMAGE.seekg(block_number * SYSTEM_BLOCK_SIZE);
    FILE_IMAGE.read(buffer, SYSTEM_BLOCK_SIZE);
}

Pointer_Block_Type check_pointer_type(char* block_content) {
    uint32_t* block_pointers = reinterpret_cast<uint32_t*>(block_content);

    for (uint32_t i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
        if (block_pointers[i] != 0) {
            // POINTED BLOCK IS NOT POINTER, THEN THIS BLOCK IS INDIRECT
            char* first_level_buffer = new char[SYSTEM_BLOCK_SIZE];
            read_block(block_pointers[i], first_level_buffer);
            if(! is_pointer_block(first_level_buffer)){
                delete[] first_level_buffer;
                return INDIRECT;
            }

            uint32_t* first_level_pointers = reinterpret_cast<uint32_t*>(first_level_buffer);
            for (uint32_t j = 0; j < SYSTEM_POINTER_PER_BLOCK; ++j) {
                if (first_level_pointers[j] != 0) {
                    char* second_level_buffer = new char[SYSTEM_BLOCK_SIZE];
                    read_block(first_level_pointers[j], second_level_buffer);

                    // POINTED BLOCK IS NOT POINTER, THEN THIS BLOCK IS DOUBLE INDIRECT
                    if(! is_pointer_block(second_level_buffer)){
                        delete[] second_level_buffer;
                        delete[] first_level_buffer;
                        return DOUBLE_INDIRECT;
                    }
                    // POINTED BLOCK IS POINTER, THEN THIS BLOCK IS TRIPLE INDIRECT
                    else{
                        delete[] second_level_buffer;
                        delete[] first_level_buffer;
                        return TRIPLE_INDIRECT;
                    }
                }
            }
        }
    }

    return NOT_POINTER_BLOCK;
}


void add_orphan_block_group_descriptor(uint32_t block_number, Block_Mode block_mode, char* block_content){
    block_descriptor bd = {0, DIRECTORY,0, NOT_POINTER_BLOCK};
    bd.block_number = block_number;
    bd.block_content_type = block_mode;
    bd.pointer_block_type = NOT_POINTER_BLOCK;
    switch (block_mode) {

        case DIRECTORY:
            bd.block_content_size = SYSTEM_BLOCK_SIZE;
            orphan_direct_directory_block_group_descriptors.push_back(bd);
            break;


        case FILE_CONTENT:
            bd.block_content_size = calculate_block_size(block_content);
            break;


        case POINTER:
            bd.pointer_block_type = check_pointer_type(block_content);
            bd.block_content_size = calculate_block_size(block_content);
            orphan_pointer_block_group_descriptors.push_back(bd);
    }
}

void sort_direct_pointers(ext2_inode * inode){
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


void handle_direct_directory_orphan_blocks() {


    if(deficient_directory_inodes.size() == 1){
        ext2_inode* inode = &deficient_directory_inodes[0].inode;
        uint32_t inode_number = deficient_directory_inodes[0].inode_number;
        int index = 0;
        for (short missing_pointer_indexes : deficient_directory_inodes[0].missing_pointer_indexes) {
            if (index < orphan_direct_directory_block_group_descriptors.size()) {
                if(orphan_direct_directory_block_group_descriptors[index].block_number == 0) {
                    continue;
                }
                if(missing_pointer_indexes == 100) {
                    break;
                }
                inode->direct_blocks[missing_pointer_indexes] = orphan_direct_directory_block_group_descriptors[index].block_number;
                index++;
            }
        }
        sort_direct_pointers(inode);
        write_one_inode(inode_number, *inode);

        // Clear vectors
        orphan_direct_directory_block_group_descriptors.clear();
        return;
    }

    char block_content[SYSTEM_BLOCK_SIZE];
    for(auto &orphan_direct_directory_block_group_descriptor : orphan_direct_directory_block_group_descriptors) {
        read_block(orphan_direct_directory_block_group_descriptor.block_number, block_content);
        uint32_t parent_inode_number = get_parent_directory_inode_number_by_directory_block(block_content);
        if(parent_inode_number){
            for(auto &deficient_directory_inode : deficient_directory_inodes){
                if(deficient_directory_inode.inode_number == parent_inode_number) {
                    ext2_inode* inode = &deficient_directory_inode.inode;
                    uint32_t inode_number = deficient_directory_inode.inode_number;
                    inode->direct_blocks[deficient_directory_inode.missing_pointer_indexes[0]] = orphan_direct_directory_block_group_descriptor.block_number;

                    for(int i = 0; i < 11; i++) {
                        deficient_directory_inode.missing_pointer_indexes[i] = deficient_directory_inode.missing_pointer_indexes[i + 1];
                    }
                    deficient_directory_inode.missing_pointer_indexes[11] = 100;
                    orphan_direct_directory_block_group_descriptor.block_number = 0;

                    sort_direct_pointers(inode);

                    write_one_inode(inode_number, *inode);
                }
            }
        }
    }


    // Remove deficient directory inodes if their missing_pointer_indexes[0] is 100
    deficient_directory_inodes.erase(
            std::remove_if(
                    deficient_directory_inodes.begin(),
                    deficient_directory_inodes.end(),
                    [](const deficient_inode& di) {
                        return di.missing_pointer_indexes[0] == 100;
                    }
            ),
            deficient_directory_inodes.end()
    );


    if(deficient_directory_inodes.size() == 1){
        ext2_inode* inode = &deficient_directory_inodes[0].inode;
        uint32_t inode_number = deficient_directory_inodes[0].inode_number;
        int index = 0;
        for (short missing_pointer_indexes : deficient_directory_inodes[0].missing_pointer_indexes) {
            if (index < orphan_direct_directory_block_group_descriptors.size()) {
                if(orphan_direct_directory_block_group_descriptors[index].block_number == 0) {
                    continue;
                }
                if(missing_pointer_indexes == 100) {
                    break;
                }
                inode->direct_blocks[missing_pointer_indexes] = orphan_direct_directory_block_group_descriptors[index].block_number;
                index++;
            }
        }
        sort_direct_pointers(inode);
        write_one_inode(inode_number, *inode);

        // Clear vectors
        orphan_direct_directory_block_group_descriptors.clear();
        return;
    }
}



void read_inodes_with_pointer_directory_block_deficiency() {
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
            for (short direct_index = 0; direct_index < min(block_count, 12); ++direct_index) {
                if(inode->direct_blocks[direct_index] == 0){
                    di.missing_pointer_indexes[missing_pointer_index] = direct_index;
                    if(direct_index != 11){
                        di.missing_pointer_indexes[missing_pointer_index + 1] = 100;
                    }
                    missing_pointer_index++;
                    is_deficient = true;
                }
            }

            // Check single indirect pointer
            if (block_count > 12) {
                if (inode->single_indirect == 0) {
                    di.is_indirect_missing = true;
                    is_deficient = true;
                }
            }

            // Check double indirect pointer
            if (block_count > 12 + SYSTEM_POINTER_PER_BLOCK) {
                if (inode->double_indirect == 0) {
                    di.is_double_indirect_missing = true;
                    is_deficient = true;
                }
            }

            // Check triple indirect pointer
            if (block_count > 12 + SYSTEM_POINTER_PER_BLOCK + SYSTEM_POINTER_PER_BLOCK * SYSTEM_POINTER_PER_BLOCK) {
                if (inode->triple_indirect == 0) {
                    di.is_triple_indirect_missing = true;
                    is_deficient = true;
                }
            }

            if(is_deficient){
                di.inode = *inode;
                di.inode_number = inode_number;

                if((inode->mode & 0xf000) == EXT2_I_DTYPE) {
                    deficient_directory_inodes.push_back(di);
                }
                else if((inode->mode & 0xf000) == EXT2_I_FTYPE){
                    deficient_file_inodes.push_back(di);
                }
            }

        }
    }
    handle_direct_directory_orphan_blocks();
}

/**
 * answer[0] = last pointer    answer[1] = index of the last pointer to calculate size
 * @param block_content
 * @return
 */
uint32_t* read_last_pointer_in_block(const char* block_content) {
    uint32_t * answer = new uint32_t[2];
    const uint32_t * pointers = reinterpret_cast<const uint32_t *>(block_content);
    for(int i = 0; i < SYSTEM_POINTER_PER_BLOCK; i++) {
        if(pointers[i] != 0){
            answer[0] = pointers[i];
            answer[1] = i;
        }
    }
    return answer;
}


void handle_indirect_orphan_blocks(block_descriptor &orphan_pointer_block_group_descriptor, char* block_content) {
    uint32_t* last_pointer_and_last_p_index = read_last_pointer_in_block(block_content);
    uint32_t last_pointer = last_pointer_and_last_p_index[0];
    uint32_t last_pointer_index = last_pointer_and_last_p_index[1];
    delete[] last_pointer_and_last_p_index; // Deallocate the array

    // Calculate the size
    uint32_t size = (last_pointer_index + 12) * SYSTEM_BLOCK_SIZE;
    orphan_pointer_block_group_descriptor.pointing_size = size;

    // Calculate the size of the last block pointed to by the last pointer
    char last_block_content[SYSTEM_BLOCK_SIZE];
    read_block(last_pointer, last_block_content);

    if(is_directory_block(last_block_content)){
        orphan_pointer_block_group_descriptor.pointing_structure = DIRECTORY;
    }
    else{
        orphan_pointer_block_group_descriptor.pointing_structure = FILE_CONTENT;
    }

    uint32_t last_block_size = calculate_block_size(last_block_content); // You need to implement calculate_block_size function
    orphan_pointer_block_group_descriptor.pointing_size += last_block_size;


    // Iterate through deficient_directory_inodes and deficient_file_inodes
    for (auto &di : deficient_directory_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size >= orphan_pointer_block_group_descriptor.pointing_size  - 8 &&
            di.inode.size <= orphan_pointer_block_group_descriptor.pointing_size  + 8 &&
            orphan_pointer_block_group_descriptor.pointing_structure == DIRECTORY) {
            // Correct the indirect pointer of the inode
            di.inode.single_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }

    for (auto &di : deficient_file_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size >= orphan_pointer_block_group_descriptor.pointing_size  - 8 &&
            di.inode.size <= orphan_pointer_block_group_descriptor.pointing_size  + 8 &&
            orphan_pointer_block_group_descriptor.pointing_structure == FILE_CONTENT) {
            // Correct the indirect pointer of the inode
            di.inode.single_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }
}

void handle_double_indirect_orphan_blocks(block_descriptor &orphan_pointer_block_group_descriptor, char* block_content) {
    uint32_t total_size = (12 + SYSTEM_POINTER_PER_BLOCK) * SYSTEM_BLOCK_SIZE;

    const uint32_t *first_level_pointers = reinterpret_cast<const uint32_t *>(block_content);
    uint32_t last_valid_first_level_pointer = 0;
    int last_valid_first_level_pointer_index = 0;
    uint32_t last_valid_second_level_pointer = 0;
    int last_valid_second_level_pointer_index = 0;

    // Iterate over the first level of pointers
    for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
        if (first_level_pointers[i] != 0) {
            last_valid_first_level_pointer = first_level_pointers[i];
            last_valid_first_level_pointer_index = i;
        }
    }

    total_size += last_valid_first_level_pointer_index * (SYSTEM_POINTER_PER_BLOCK) * SYSTEM_BLOCK_SIZE;

    if (last_valid_first_level_pointer != 0) {
        char second_level_block_content[SYSTEM_BLOCK_SIZE];
        read_block(last_valid_first_level_pointer, second_level_block_content);

        const uint32_t *second_level_pointers = reinterpret_cast<const uint32_t *>(second_level_block_content);
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if (second_level_pointers[i] != 0) {
                last_valid_second_level_pointer = second_level_pointers[i];
                last_valid_second_level_pointer_index = i;
            }
        }

        if (last_valid_second_level_pointer != 0) {
            char last_block_content[SYSTEM_BLOCK_SIZE];
            read_block(last_valid_second_level_pointer, last_block_content);

            if (is_directory_block(last_block_content)) {
                orphan_pointer_block_group_descriptor.pointing_structure = DIRECTORY;
            } else {
                orphan_pointer_block_group_descriptor.pointing_structure = FILE_CONTENT;
            }

            total_size += last_valid_second_level_pointer_index * SYSTEM_BLOCK_SIZE;
            uint32_t last_block_size = calculate_block_size(last_block_content);
            total_size += last_block_size;
        }

    }

    orphan_pointer_block_group_descriptor.pointing_size = total_size;

    // Iterate through deficient_directory_inodes and deficient_file_inodes
    for (auto &di : deficient_directory_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size >= orphan_pointer_block_group_descriptor.pointing_size - 8 &&
            di.inode.size <= orphan_pointer_block_group_descriptor.pointing_size + 8 &&
            orphan_pointer_block_group_descriptor.pointing_structure == DIRECTORY) {
            // Correct the double indirect pointer of the inode
            di.inode.double_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_double_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }

    for (auto &di : deficient_file_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size >= orphan_pointer_block_group_descriptor.pointing_size - 8 &&
            di.inode.size <= orphan_pointer_block_group_descriptor.pointing_size + 8 &&
            orphan_pointer_block_group_descriptor.pointing_structure == FILE_CONTENT) {
            // Correct the double indirect pointer of the inode
            di.inode.double_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_double_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }
}


// MUALLAK
void handle_triple_indirect_orphan_blocks(block_descriptor &orphan_pointer_block_group_descriptor, char* block_content) {
    uint32_t total_size = (12 + SYSTEM_POINTER_PER_BLOCK + SYSTEM_POINTER_PER_BLOCK * SYSTEM_POINTER_PER_BLOCK) * SYSTEM_BLOCK_SIZE;

    const uint32_t *first_level_pointers = reinterpret_cast<const uint32_t *>(block_content);
    uint32_t last_valid_first_level_pointer = 0;
    int last_valid_first_level_pointer_index = 0;
    uint32_t last_valid_second_level_pointer = 0;
    int last_valid_second_level_pointer_index = 0;
    uint32_t last_valid_third_level_pointer = 0;
    int last_valid_third_level_pointer_index = 0;

    // Iterate over the first level of pointers
    for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
        if (first_level_pointers[i] != 0) {
            last_valid_first_level_pointer = first_level_pointers[i];
            last_valid_first_level_pointer_index = i;
        }
    }

    total_size += last_valid_first_level_pointer_index * (SYSTEM_POINTER_PER_BLOCK) * (SYSTEM_POINTER_PER_BLOCK) * SYSTEM_BLOCK_SIZE;

    if (last_valid_first_level_pointer != 0) {
        char second_level_block_content[SYSTEM_BLOCK_SIZE];
        read_block(last_valid_first_level_pointer, second_level_block_content);

        const uint32_t *second_level_pointers = reinterpret_cast<const uint32_t *>(second_level_block_content);
        for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
            if (second_level_pointers[i] != 0) {
                last_valid_second_level_pointer = second_level_pointers[i];
                last_valid_second_level_pointer_index = i;
            }
        }

        total_size += last_valid_second_level_pointer_index * (SYSTEM_POINTER_PER_BLOCK) * SYSTEM_BLOCK_SIZE;

        if (last_valid_second_level_pointer != 0) {
            char third_level_block_content[SYSTEM_BLOCK_SIZE];
            read_block(last_valid_second_level_pointer, third_level_block_content);

            const uint32_t *third_level_pointers = reinterpret_cast<const uint32_t *>(third_level_block_content);
            for (int i = 0; i < SYSTEM_POINTER_PER_BLOCK; ++i) {
                if (third_level_pointers[i] != 0) {
                    last_valid_third_level_pointer = third_level_pointers[i];
                    last_valid_third_level_pointer_index = i;
                }
            }

            total_size += last_valid_third_level_pointer_index * SYSTEM_BLOCK_SIZE;

            if (last_valid_third_level_pointer != 0) {
                char last_block_content[SYSTEM_BLOCK_SIZE];
                read_block(last_valid_third_level_pointer, last_block_content);

                if (is_directory_block(last_block_content)) {
                    orphan_pointer_block_group_descriptor.pointing_structure = DIRECTORY;
                } else {
                    orphan_pointer_block_group_descriptor.pointing_structure = FILE_CONTENT;
                }

                uint32_t last_block_size = calculate_block_size(last_block_content);
                total_size += last_block_size;
            }
        }
    }

    orphan_pointer_block_group_descriptor.pointing_size = total_size;

    // Iterate through deficient_directory_inodes and deficient_file_inodes
    for (auto &di : deficient_directory_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size >= orphan_pointer_block_group_descriptor.pointing_size - 8 &&
            di.inode.size <= orphan_pointer_block_group_descriptor.pointing_size + 8 &&
            orphan_pointer_block_group_descriptor.pointing_structure == DIRECTORY) {
            // Correct the triple indirect pointer of the inode
            di.inode.triple_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_triple_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }

    for (auto &di : deficient_file_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size >= orphan_pointer_block_group_descriptor.pointing_size - 8 &&
            di.inode.size <= orphan_pointer_block_group_descriptor.pointing_size + 8 &&
            orphan_pointer_block_group_descriptor.pointing_structure == FILE_CONTENT) {
            // Correct the triple indirect pointer of the inode
            di.inode.triple_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_triple_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }
}


void handle_residual_indirect_orphan_blocks(block_descriptor &orphan_pointer_block_group_descriptor, char* block_content) {
    // Iterate through deficient_directory_inodes and deficient_file_inodes
    for (auto &di : deficient_directory_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size > ((12 + SYSTEM_POINTER_PER_BLOCK) * SYSTEM_BLOCK_SIZE) &&
            orphan_pointer_block_group_descriptor.pointing_structure == DIRECTORY) {
            if(di.inode.direct_blocks[11] == 0 || di.inode.single_indirect != 0) {
                continue;
            }

            // Correct the indirect pointer of the inode
            di.inode.single_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }

    for (auto &di : deficient_file_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size > ((12 + SYSTEM_POINTER_PER_BLOCK) * SYSTEM_BLOCK_SIZE) &&
            orphan_pointer_block_group_descriptor.pointing_structure == FILE_CONTENT) {
            if(di.inode.direct_blocks[11] == 0  || di.inode.single_indirect != 0) {
                continue;
            }
            // Correct the indirect pointer of the inode
            di.inode.single_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }
}


void handle_residual_double_indirect_orphan_blocks(block_descriptor &orphan_pointer_block_group_descriptor, char* block_content) {
    // Iterate through deficient_directory_inodes and deficient_file_inodes
    for (auto &di : deficient_directory_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size > (12 + SYSTEM_POINTER_PER_BLOCK + (SYSTEM_POINTER_PER_BLOCK * SYSTEM_POINTER_PER_BLOCK)) * SYSTEM_BLOCK_SIZE &&
            orphan_pointer_block_group_descriptor.pointing_structure == DIRECTORY) {
            if(di.is_double_indirect_missing != 0 && di.inode.single_indirect == 0) {
                continue;
            }

            // Correct the double indirect pointer of the inode
            di.inode.double_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_double_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }

    for (auto &di : deficient_file_inodes) {
        if (orphan_pointer_block_group_descriptor.block_number != 0 &&
            di.inode.size > (12 + SYSTEM_POINTER_PER_BLOCK + (SYSTEM_POINTER_PER_BLOCK * SYSTEM_POINTER_PER_BLOCK)) * SYSTEM_BLOCK_SIZE &&
            orphan_pointer_block_group_descriptor.pointing_structure == FILE_CONTENT) {
            if(di.is_double_indirect_missing != 0 && di.inode.single_indirect == 0) {
                continue;
            }
            // Correct the double indirect pointer of the inode
            di.inode.double_indirect = orphan_pointer_block_group_descriptor.block_number;
            di.is_double_indirect_missing = false;

            // Nullify block number
            orphan_pointer_block_group_descriptor.block_number = 0;

            // Write the corrected inode back to the file
            write_one_inode(di.inode_number, di.inode);
        }
    }
}



void calculate_orphan_block_sizes_and_correct_inodes() {
    for (auto & orphan_pointer_block_group_descriptor : orphan_pointer_block_group_descriptors) {
        if(orphan_pointer_block_group_descriptor.block_number == 0) {
            continue;
        }
        uint32_t block_number = orphan_pointer_block_group_descriptor.block_number;

        // Read the block content
        char block_content[SYSTEM_BLOCK_SIZE];
        read_block(block_number, block_content);
        // Determine the type of pointer block
        if(orphan_pointer_block_group_descriptor.pointer_block_type == INDIRECT) {
            handle_indirect_orphan_blocks(orphan_pointer_block_group_descriptor, block_content);
        } else if(orphan_pointer_block_group_descriptor.pointer_block_type == DOUBLE_INDIRECT){
            handle_double_indirect_orphan_blocks(orphan_pointer_block_group_descriptor, block_content);
        }
        else if(orphan_pointer_block_group_descriptor.pointer_block_type == TRIPLE_INDIRECT) {
            handle_triple_indirect_orphan_blocks(orphan_pointer_block_group_descriptor, block_content);
        }
    }


    for (auto & orphan_pointer_block_group_descriptor : orphan_pointer_block_group_descriptors) {
        if(orphan_pointer_block_group_descriptor.block_number == 0) {
            continue;
        }
        uint32_t block_number = orphan_pointer_block_group_descriptor.block_number;

        // Read the block content
        char block_content[SYSTEM_BLOCK_SIZE];
        read_block(block_number, block_content);
        // Determine the type of pointer block
        if(orphan_pointer_block_group_descriptor.pointer_block_type == INDIRECT) {
            handle_residual_indirect_orphan_blocks(orphan_pointer_block_group_descriptor, block_content);
        } else if(orphan_pointer_block_group_descriptor.pointer_block_type == DOUBLE_INDIRECT){
            handle_residual_double_indirect_orphan_blocks(orphan_pointer_block_group_descriptor, block_content);
        }
    }

    // Remove orphan pointer block descriptors with block number 0
    orphan_pointer_block_group_descriptors.erase(
            std::remove_if(
                    orphan_pointer_block_group_descriptors.begin(),
                    orphan_pointer_block_group_descriptors.end(),
                    [](const block_descriptor& bd) {
                        return bd.block_number == 0;
                    }
            ),
            orphan_pointer_block_group_descriptors.end()
    );
}