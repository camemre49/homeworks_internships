#ifndef FILE_HW3_DATA_BITMAP_HANDLER_H
#define FILE_HW3_DATA_BITMAP_HANDLER_H


void set_data_blocks_pointed_in_inodes();
void set_data_blocks_by_traverse();
void set_redundant_data_bitmap_bits();
void combine_data_bitmaps();
bool is_pointer_block(char* block_content);
bool is_file_content_block(char* block_content);

#endif //FILE_HW3_DATA_BITMAP_HANDLER_H
