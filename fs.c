/*
  gcc -Wall fs.c `pkg-config fuse --cflags --libs` -o fusefs
*/

/*
http://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/unclear.html
http://fuse.sourceforge.net/doxygen/structfuse__operations.html

man errno.h
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "src/userfs.h"
#include "src/dir.h"
#include "src/inode.h"
#include "src/blocks.h"
#include "src/sb.h"
#include "src/bitmap.h"
#include "fs.h"

#define MAX(a,b) ((a<b)?b:a)

/* Sets stbuf's properties based on file path
   man 3 stat
   man stat.h
   
   st_mode      Filemode 0666 rw-rw-rw
   st_nlink     Number of hard links to the file (1)
   st_mtime     Modified Time
   st_ctime     Created Time
   st_size      File size in bytes
*/
static int fs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	file_t * f;	
	inode_t in;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		stbuf->st_mtime = time(NULL);
		stbuf->st_ctime = time(NULL);
	}
	else if (dir_find_file(path, &f)) {
	// TODO: make this proper
		inode_read(f->inode_number, &in);
		fprintf(stderr, "Found file: %s\n", f->file_name);
		stbuf->st_mode = f->mode;
		stbuf->st_nlink = 1;
		stbuf->st_mtime = time(NULL);
		stbuf->st_ctime = time(NULL);
		stbuf->st_size = in.file_size_bytes;
		stbuf->st_blocks = in.num_blocks;
		stbuf->st_blksize = BLOCK_SIZE_BYTES;
		stbuf->st_ino = f->inode_number;
		stbuf->st_uid = f->uid;
		stbuf->st_gid = f->gid;
	}
	else {
		fprintf(stderr, "WTFFFFF\n");
		res = -ENOENT;
	}
	
	return res;
}

/* Reads all of the files in path into buf using the filler function
   int (*fuse_fill_dir_t)(void *buffer, char* filename, NULL, int offset 0);
   	filler will add a file to the result buffer (buf)
   
*/
static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	int i;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	for (i = 0; i < MAX_FILES_PER_DIRECTORY; i++) {
		if (root_dir.u_file[i].free) continue;
		fprintf(stderr, "trying to fill '%s'\n", root_dir.u_file[i].file_name);
		filler(buf, root_dir.u_file[i].file_name+1, NULL, 0);
	}

	return 0;
}

/* Create a empty file named path 
   
   Find a free inode
   Mark it as allocated
   Allocate file as our inode in root dir
   Writes relevent blocks
*/
static int fs_create(const char *path, mode_t mode, struct fuse_file_info * fi)
{
	int inum;
	inode_t in;	
	file_t * f;

	if(strlen(path) > MAX_FILE_NAME_SIZE) {
		return -ENAMETOOLONG;
	}
	
	if(dir_is_full()) {
		return -1;
	}

	if (dir_find_file(path, &f)) {
		return -EEXIST;
	}	

	inum = inode_next();
	if (inum == -1) {
		fprintf(stderr, "~~~ Invalid inode returned ~~~\n");
		return -1;
	}

	inode_alloc(&in, 0, 0); // TODO Fix this
	inode_write(inum, &in);
	dir_allocate_file(inum, path);

	return 0;
}

/* Checks that a file can be opened */
static int fs_open(const char *path, struct fuse_file_info *fi)
{
	file_t * file;
	
	if (dir_find_file(path, &file)) {
		return 0;
	}

	return -1;
}

/* Reads the contents of file into buf
   man 3 read
   
   finds the file for path
   reads contents of file into buf starting at offset  (recommended: implement offset after you have the rest working)
   returns the number of bytes read
*/
static int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	
	int i;
	int read_bytes;
	int current_bytes;
	int off;
	inode_t inode;
	
	file_t * file;
	//FUSE Should have called open to check that the file exists ahead of time
	assert(dir_find_file(path, &file));
	read_bytes = 0;
	inode_read(file->inode_number, &inode);
	off = offset % BLOCK_SIZE_BYTES; 

	for (i = offset/BLOCK_SIZE_BYTES; i < inode.num_blocks; i++) {	
		fprintf(stderr, "reading from block %d\n", inode.blocks[i]);
		block_read_offset(inode.blocks[i], buf+read_bytes, MAX(BLOCK_SIZE_BYTES, size-read_bytes), off);
		read_bytes += MAX(BLOCK_SIZE_BYTES, size-read_bytes);
		if (read_bytes == size) break;
		off = 0;
	}
	
	return read_bytes;
}

/* Writes contents of buf to file
   man 3 write
   
   find file for path
   writes contents of buf into file starting at offset  (recommended: implement offset after you have the rest working)
      figure out how many blocks you need
      for each required block
      	find a free block
      	update inode to have new block
		write buf to blocks
	write relevent blocks
*/
static int fs_write(const char * path, const char * buf, size_t buff_size, off_t offset, struct fuse_file_info * fi) {
	inode_t inode;
	int i;
	int new_size;
	int new_blockno;
	DISK_LBA current_block;	
	int buf_rem;
	int wblki;
	
	file_t * file;
	assert(dir_find_file(path, &file));
	
	inode_read(file->inode_number, &inode);

	// Check if we need to resize the file.
	if (inode.file_size_bytes < buff_size + offset) {
		new_size = buff_size + offset;
		new_blockno = floor(new_size/BLOCK_SIZE_BYTES) + 1;

		if (new_blockno - inode.num_blocks > u_quota()) {
			return -ENOSPC;
		}
		
		if (!valid_file_size(new_blockno)) {
			return -EFBIG;
		}

		for (i = inode.num_blocks; i < new_blockno; i++) {
			inode.blocks[i] = block_find();
			block_alloc(inode.blocks[i]);
		}


		inode.file_size_bytes = new_size;
		inode.num_blocks = new_blockno;
		inode_write(file->inode_number, &inode);
	}
	
	buf_rem = buff_size;
	wblki = offset / BLOCK_SIZE_BYTES;
	while (buf_rem > 0) {
		assert(wblki < inode.num_blocks);
		fprintf(stderr, "writing to block %d\n", inode.blocks[wblki]);
		block_write_offset(inode.blocks[wblki], buf + (buff_size-buf_rem), MAX(buf_rem, BLOCK_SIZE_BYTES), offset);
		wblki++; // Write to the next block on next iteration
		buf_rem -= BLOCK_SIZE_BYTES;
		offset = 0; // Only need to write offset once, clear for writes to other blocks
	}

	return buff_size;
}

/* Trims file to offset length
   figure out which blocks to free
   free relevent blocks
   update inode
*/
static int fs_truncate(const char * path, off_t offset) {
	inode_t inode;
	int i;
	int fh;
	int startblock;
	DISK_LBA current_block;
	
	file_t * file;
	assert(dir_find_file(path, &file));
	inode_read(file->inode_number, &inode);

	inode.file_size_bytes = offset;
	inode.num_blocks = offset / BLOCK_SIZE_BYTES; // Last block needed
	inode.num_blocks += (offset == 0) ? 0 : 1;

	for (i = inode.num_blocks; i < inode.num_blocks; i++) {
		block_free(inode.blocks[i]);
	}

	inode_write(file->inode_number, &inode);

// TODO: allow forward truncation?

	return 0;
}

/* Remove file 
   Save relevent blocks
*/
int fs_unlink(const char * path) {
	file_t * file;
	if (dir_find_file(path, &file)) {
		dir_remove_file(file);
		/* TODO write blocks */
		return 0;
	}
	return -ENOENT;
}

//Extra credit
static int fs_chown(const char * path, uid_t uid, gid_t gid) {

	file_t * f;

	if (!dir_find_file(path, &f)) {
		return -ENOENT;
	}

	f->uid = uid;
	f->gid = gid;

	dir_write();

	return 0;
}

//Extra credit
static int fs_chmod(const char * path, mode_t mode) {
	
	file_t * f;

	if (!dir_find_file(path, &f)) {
		return -ENOENT;
	}

	f->mode = mode;	

	dir_write();
	
	return 0;
}

//Extra credit
static int fs_utimens(const char * path, const struct timespec tv[2] ) {
	return 0;
}

static int fs_rename(const char * oldpath, const char * newpath) {
	file_t * file;
	
	if(strlen(newpath) > MAX_FILE_NAME_SIZE) {
		return -ENAMETOOLONG;
	}
	
	if (dir_find_file(oldpath, &file)) {
		dir_rename_file(oldpath, newpath);
		return 0;
	}
	return -ENOENT;
}

//Creates a structure to tell fuse about the operations we have implemented
static struct fuse_operations fs_oper = {
	.getattr  = fs_getattr,
	.readdir  = fs_readdir,
	.open     = fs_open,
	.read     = fs_read,
	.create   = fs_create,
	.chown    = fs_chown,
	.chmod    = fs_chmod,
	.utimens  = fs_utimens,
	.truncate = fs_truncate,
	.write    = fs_write,
	.unlink   = fs_unlink,
	.rename   = fs_rename,
};

int u_quota() {
	int freeCount=0;
	int i;
	
	assert(BIT_MAP_SIZE > sb.disk_size_blocks);
	
	for (i=0; i < sb.disk_size_blocks; i++ )
	{
		if (bit_map[i]==0)
		{
			freeCount++;
		}
	}
	return freeCount;
}

int main(int argc, char **argv)
{
	int ret;
	char ** fuse_argv;
	int fuse_argc = 0;
	
	bool do_format = false;
	int size_format = 0;
	
	int argi;
	char * arg;
	char * disk = NULL;
	
	bool disable_crash = false;
	
	//Copy prog name
	fuse_argv = malloc(sizeof(char *) * argc);
	fuse_argv[0] = argv[0];
	fuse_argc++;
	
	for (argi = 1; argi < argc; argi++) {
		arg = argv[argi];
		
		if (strcmp(arg, "--help") == 0) {
			printf("Usage:\n");
			printf("\t--disk [diskfile]\n");
			printf("\t--format [size]\n");
			printf("\t--no-crash\n");
			printf("\t--help\n");
			return 0;
		} else if (strcmp(arg, "--disk") == 0) {
			argi++;
			disk = argv[argi];
		} else if (strcmp(arg, "--format") == 0) {
			do_format = true;
			argi++;
			size_format = atoi(argv[argi]);
		} else if (strcmp(arg, "--no-crash") == 0) {
			disable_crash = true;
		} else {
			fuse_argv[fuse_argc] = arg;
			fuse_argc++;
		}
	}
	
	if (disk == NULL) {
		fprintf(stderr, "Must specify disk\n");
		return -1;
	}
	
	if (do_format) {
		fprintf(stderr, "Formatting %s (size %i)\n", disk, size_format);
		util_format(size_format, disk);
		return 0;
	}
	
	util_recover_fs(disk);
	//We are not clean
	sb.clean_shutdown = 0;
	
	if (!disable_crash) {
		init_crasher();
	}
	
	// TODO: Move this into format?
	dir_init();
	dir_read();

	ret = fuse_main(fuse_argc, fuse_argv, &fs_oper, NULL);
	//We are unmounted. clean shutdown
	fprintf(stderr, "Clean shutting down...\n");
	util_clean_shutdown();
	
	free(fuse_argv);
	return ret;
}
