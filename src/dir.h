#ifndef U_DIR
#define U_DIR

#define MAX_FILES_PER_DIRECTORY 100
#define DIRECTORY_BLOCK 2

#include "file.h"

typedef struct dir_struct_s {
	int num_files;
	file_struct u_file[MAX_FILES_PER_DIRECTORY];
} dir_t;

void dir_init(void);
void dir_allocate_file(int, const char *);
void dir_write(void);
bool dir_full(void);
bool dir_find_file(const char *, file_struct *);
void dir_remove_file(file_struct);
void dir_rename_file(const char *, const char *);

dir_t root_dir;

#endif
