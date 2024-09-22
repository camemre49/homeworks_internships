#ifndef FILE_HW3_INODE_BITMAP_HANDLER_H
#define FILE_HW3_INODE_BITMAP_HANDLER_H

#include "ext2fs.h"

void inode_consistency_check(bool assume_pointers_intact);
bool single_inode_consistency_check(ext2_inode inode, uint32_t inode_number, bool assume_pointers_intact);
void deallocate_inode_bitmap_replicate();
void combine_inode_bitmaps();

#endif //FILE_HW3_INODE_BITMAP_HANDLER_H
