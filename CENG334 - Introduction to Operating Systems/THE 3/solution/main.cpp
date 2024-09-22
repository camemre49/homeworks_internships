#include <cstring>
#include "global_vars.h"
#include "directory_table_handler.h"
#include "inode_bitmap_handler.h"
#include "data_bitmap_handler.h"
#include "write_handler.h"
#include "identifier.h"
#include "pointer-recovery-handler.h"
#include "residual_file_content_handler.h"


int main(int argc, char* argv[]) {
    const char* image_path = argv[1]; // Get the image path from command line argument
    IDENTIFIER = parse_identifier(argc, argv);
    IMAGE_PATH = new char[std::strlen(argv[1]) + 1];
    std::strcpy(IMAGE_PATH, argv[1]);


    read_file_image(image_path);

    // data bitmap recovery
    set_data_blocks_pointed_in_inodes();
    set_redundant_data_bitmap_bits();
    set_data_blocks_by_traverse();
    combine_data_bitmaps();


    //inode bitmap recovery
    inode_consistency_check(false);
    combine_inode_bitmaps();

    // Write bitmaps to the file
    write_bitmaps(image_path);

    // pointer recovery
    read_inodes_with_pointer_directory_block_deficiency();
    calculate_orphan_block_sizes_and_correct_inodes();

    // These blocks are all direct blocks, other blocks are handler before
    recover_residual_file_content_blocks();

    // print directory structure by traversing from root
    read_inode_tables_by_group_index_no_print(0);
    print_directory_structure();



    FILE_IMAGE.close();
    delete SUPER_BLOCK;
    deallocate_group_descriptors();
    deallocate_bitmaps();
    delete [] LAST_READ_INODE_TABLE;
    deallocate_file_content();
    deallocate_inode_bitmap_replicate();
    delete [] IMAGE_PATH;
    return 1;
}
