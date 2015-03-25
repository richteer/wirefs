#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "userfs.h"
#include "blocks.h"
#include "file.h"
#include "dir.h"
#include "inode.h"

bool dir_is_full(void)
{
	return root_dir.num_files == MAX_FILES_PER_DIRECTORY;
}

void dir_init(void)
{
	int i;

	root_dir.num_files = 0;

	for (i = 0; i < MAX_FILES_PER_DIRECTORY; i++) {
		root_dir.u_file[i].free = 5;
	}

}

void dir_write(void)
{
	block_write(DIRECTORY_BLOCK, &root_dir, sizeof(dir_t));

}

void dir_allocate_file(int inode, const char * name)
{
	int i;

	if (dir_is_full()) return;

	for (i = 0; i < MAX_FILES_PER_DIRECTORY; i++) {
		if (root_dir.u_file[i].free) break;
	}

	root_dir.u_file[i].free = false;
	strcpy(root_dir.u_file[i].file_name,name);
	root_dir.u_file[i].inode_number = inode;

}


bool dir_find_file(const char * name, file_t ** file)
{
	int i;

	for (i = 0; i < MAX_FILES_PER_DIRECTORY; i++) {
		if (!root_dir.u_file[i].free) continue;
		if (!strcmp(root_dir.u_file[i].file_name, name)) {
			*file = &root_dir.u_file[i];
			return true;
		}
	}

	*file = NULL;
	return false;
}

void dir_remove_file(file_t * file)
{
	inode_t in;

	file->free = true;
	file->file_name[0] = '\0';
	inode_read(file->inode_number, &in);
	in.free = true;
	// TODO: free the blocks
	inode_write(file->inode_number, &in);

}

void dir_rename_file(const char * old, const char * new)
{
	int i;
	file_t * f;

	if (!dir_find_file(old, &f)) return;

	strcpy(f->file_name, new);

}
