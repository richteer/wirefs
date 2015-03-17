#ifndef U_BLOCKS
#define U_BLOCKS

#define BLOCK_SIZE_BYTES 4096

void allocate_block(DISK_LBA);
void free_block(DISK_LBA);
int find_block();
void write_block(DISK_LBA, const void *, int);
void write_block_offset(DISK_LBA block, const void * data, int size, int offset);
void read_block(DISK_LBA, void *, int);

#endif
