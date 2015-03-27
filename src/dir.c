#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "userfs.h"
#include "blocks.h"
#include "file.h"
#include "dir.h"
#include "inode.h"

dir_t root_dir = {0};

bool dir_is_full(void)
{
	return root_dir.num_files == MAX_FILES_PER_DIRECTORY;
}

void dir_init(void)
{
	int i;

	root_dir.num_files = 0;

	for (i = 0; i < MAX_FILES_PER_DIRECTORY; i++) {
		root_dir.u_file[i].free = true;
	}

}

void dir_read(void)
{
	fprintf(stderr, "reading\n");
	block_read(DIRECTORY_BLOCK, &root_dir, sizeof(dir_t));
	fprintf(stderr, "reading again\n");
}

void dir_write(void)
{
	fprintf(stderr, "writing\n");
	block_write(DIRECTORY_BLOCK, &root_dir, sizeof(dir_t));
	fprintf(stderr, "writing again\n");
}

void dir_allocate_file(int inode, const char * name)
{
	int i;

	if (dir_is_full()) return;

	for (i = 0; i < MAX_FILES_PER_DIRECTORY; i++) {
		if (root_dir.u_file[i].free) break;
	}

	root_dir.u_file[i].free = false;
	strcpy(root_dir.u_file[i].file_name, name);
	root_dir.u_file[i].inode_number = inode;
	
	root_dir.num_files++;

	dir_write();
}


bool dir_find_file(const char * name, file_t ** file)
{
	int i;

	dir_read();

	for (i = 0; i < MAX_FILES_PER_DIRECTORY; i++) {
		if (root_dir.u_file[i].free) continue;
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
	int i;
	inode_t in;

	file->free = true;
	file->file_name[0] = '\0';
	inode_read(file->inode_number, &in);
	in.free = true;

	for (i = 0; i < MAX_BLOCKS_PER_FILE; i++) {
		block_free(in.blocks[i]);
	}
	
	inode_write(file->inode_number, &in);

	dir_write();
}

void dir_rename_file(const char * old, const char * new)
{
	int i;
	file_t * f;

	if (!dir_find_file(old, &f)) return;

	strcpy(f->file_name, new);

	dir_write();
}
