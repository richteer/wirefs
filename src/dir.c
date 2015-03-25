#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "userfs.h"
#include "blocks.h"
#include "file.h"
#include "dir.h"
#include "inode.h"

void dir_init(void)
{
	int i;

	root_dir.num_files = 0;

	for (i=0; i< MAX_FILES_PER_DIRECTORY; i++) {
		root_dir.u_file[i].free = 5;
	}
}

void dir_write(void)
{
	write_block(DIRECTORY_BLOCK, &root_dir, sizeof(dir_t));
}

// TODO Finds a free file and allocates it, sets the free file's inode and name
void dir_allocate_file(int inode, const char * name)
{
}

// TODO: write dir_is_full
bool dir_is_full(void)
{
}

// TODO Finds the file specified by name sets the file parameter to the file that was found
bool dir_find_file(const char * name, file_struct * file)
{
	return false;
}

// TODO: Free file's blocks, Free file's inode, Free file
void dir_remove_file(file_struct file)
{
}

// TODO write rename file
void dir_rename_file(const char * old, const char * new)
{
}
