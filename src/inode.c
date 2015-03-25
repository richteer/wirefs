#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include "userfs.h"
#include "crash.h"
#include "blocks.h"
#include "inode.h"


int inode_get_location(int inode_number)
{
	int whichInodeBlock;
	int whichInodeInBlock;

	whichInodeBlock = inode_number/INODES_PER_BLOCK;
	whichInodeInBlock = inode_number%INODES_PER_BLOCK;

	return (INODE_BLOCK + whichInodeBlock) *BLOCK_SIZE_BYTES + whichInodeInBlock*sizeof(inode_t);
}

int inode_write(int inode_number, inode_t * in)
{
	int inodeLocation;
	assert(inode_number < MAX_INODES);

	inodeLocation = inode_get_location(inode_number);
  	in->last_modified = time(NULL);
	lseek(virtual_disk, inodeLocation, SEEK_SET);
	crash_write(virtual_disk, in, sizeof(inode_t));
  
	sync();

	return 1;
}


int inode_read(int inode_number, inode_t * in) 
{
	int inodeLocation;
	assert(inode_number < MAX_INODES);

	inodeLocation = inode_get_location(inode_number);

  
	lseek(virtual_disk, inodeLocation, SEEK_SET);
	read(virtual_disk, in, sizeof(inode_t));
  
	return 1;
}

void inode_alloc(inode_t * in, int blocks, int size)
{
	in->num_blocks = blocks;
	in->file_size_bytes = size;
	in->free = 0;
	in->last_modified = time(NULL);
}

int inode_next()
{
	int i;
	inode_t in;

	for (i = 0; i < MAX_INODES; i++) {
		inode_read(i, &in);
		if (in.free) return i;
	}

	return -1;
}

