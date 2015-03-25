#ifndef U_BLOCKS
#define U_BLOCKS

#define BLOCK_SIZE_BYTES 4096

void block_alloc(DISK_LBA);
void block_free(DISK_LBA);
int block_find();
void block_write(DISK_LBA, const void *, int);
void block_write_offset(DISK_LBA block, const void * data, int size, int offset);
void block_read(DISK_LBA, void *, int);

#endif
