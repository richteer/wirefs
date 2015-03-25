#ifndef U_INODE
#define U_INODE

#define INODE_BLOCK 3
#define MAX_BLOCKS_PER_FILE	100
#define INODES_PER_BLOCK (BLOCK_SIZE_BYTES/sizeof(inode))
#define MAX_INODES (INODES_PER_BLOCK * NUM_INODE_BLOCKS)
#define NUM_INODE_BLOCKS 5

#include <time.h>
#include "inode.h"
#include "userfs.h"

typedef struct i_node {
	int no_blocks;
	int file_size_bytes;
	time_t last_modified; // optional add other information
	DISK_LBA blocks[MAX_BLOCKS_PER_FILE];
	bool free;
} inode;

int inode_get_location(int);
int inode_write(int, inode *);
int inode_read(int, inode *);
void inode_alloc(inode *, int, int);
int inode_next();

#endif

