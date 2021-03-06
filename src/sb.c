#include <stdbool.h>
#include <time.h>
#include "userfs.h"
#include "blocks.h"
#include "inode.h"
#include "file.h"
#include "dir.h"
#include "sb.h"
#include "stdbool.h"

int superblock_matches(void)
{
	return   (sb.size_of_super_block == sizeof(superblock))
		&& (sb.size_of_directory == sizeof (dir_t))
		&& (sb.size_of_inode == sizeof(inode_t))
		&& (sb.block_size_bytes == BLOCK_SIZE_BYTES)
		&& (sb.max_file_name_size == MAX_FILE_NAME_SIZE)
		&& (sb.max_blocks_per_file == MAX_BLOCKS_PER_FILE);
}

void superblock_init(int diskSizeBytes)
{
	sb.disk_size_blocks  = diskSizeBytes/BLOCK_SIZE_BYTES;
	sb.num_free_blocks = u_quota();
	sb.clean_shutdown = 1;

	sb.size_of_super_block = sizeof(superblock);
	sb.size_of_directory = sizeof (dir_t);
	sb.size_of_inode = sizeof(inode_t);

	sb.block_size_bytes = BLOCK_SIZE_BYTES;
	sb.max_file_name_size = MAX_FILE_NAME_SIZE;
	sb.max_blocks_per_file = MAX_BLOCKS_PER_FILE;
}
