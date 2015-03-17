#ifndef U_FS
#define U_FS
static int fs_getattr(const char *path, struct stat *stbuf);
static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int fs_open(const char *path, struct fuse_file_info *fi);
static int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int fs_write(const char * path, const char * buf, size_t buff_size, off_t offset, struct fuse_file_info * fi);
static int fs_truncate(const char * path, off_t offset);
int fs_unlink(const char * path);

static int fs_chown(const char * path, uid_t uid, gid_t gid);
static int fs_chmod(const char * path, mode_t mode);
static int fs_utimens(const char * path, const struct timespec tv[2] );
#endif
