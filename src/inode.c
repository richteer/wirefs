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

	return (INODE_BLOCK + whichInodeBlock) *BLOCK_SIZE_BYTES + whichInodeInBlock*sizeof(inode);
}

int inode_write(int inode_number, inode * in)
{
	int inodeLocation;
	assert(inode_number < MAX_INODES);

	inodeLocation = inode_get_location(inode_number);
  	in->last_modified = time(NULL);
	lseek(virtual_disk, inodeLocation, SEEK_SET);
	crash_write(virtual_disk, in, sizeof(inode));
  
	sync();

	return 1;
}


int inode_read(int inode_number, inode * in) 
{
	int inodeLocation;
	assert(inode_number < MAX_INODES);

	inodeLocation = inode_get_location(inode_number);

  
	lseek(virtual_disk, inodeLocation, SEEK_SET);
	read(virtual_disk, in, sizeof(inode));
  
	return 1;
}

/* TODO
   Sets an inode as allocated
*/
void inode_alloc(inode * in, int blocks, int size) {
}

/* TODO
   Returns the next free inode.
*/
int inode_next() {
	return -1;
}

