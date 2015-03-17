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

typedef struct i_node{
	int no_blocks;
	int file_size_bytes;
	time_t last_modified; // optional add other information
	DISK_LBA blocks[MAX_BLOCKS_PER_FILE];
	bool free;
}inode;

int compute_inode_loc(int);
int write_inode(int , inode *);
int read_inode(int , inode *);
void allocate_inode(inode *, int, int);
int free_inode();

#endif

