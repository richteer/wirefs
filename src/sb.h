#ifndef U_SB
#define U_SB

#define SUPERBLOCK_BLOCK 0

typedef struct superblock_s {
	int size_of_super_block;
	int size_of_directory;
	int size_of_inode;

	int disk_size_blocks;
	int num_free_blocks;

	int block_size_bytes;
	int max_file_name_size;
	int max_blocks_per_file;

	bool clean_shutdown; //if true can assume numFreeBlocks is valid

} superblock;

superblock sb;

int superblockMatchesCode();
void init_superblock();

#endif

