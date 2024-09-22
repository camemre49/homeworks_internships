#ifndef FILE_HW3_WRITE_HANDLER_H
#define FILE_HW3_WRITE_HANDLER_H


void write_bitmaps(const char* image_path);
void write_one_inode(uint32_t inode_number, ext2_inode &inode);

#endif //FILE_HW3_WRITE_HANDLER_H
