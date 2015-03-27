#ifndef U_FILE
#define U_FILE

#include <sys/types.h>

#define MAX_FILE_NAME_SIZE 15

typedef struct file_struct_s {
	int inode_number;
	char file_name[MAX_FILE_NAME_SIZE+1];
	mode_t mode;
	bool free;
} file_t;

bool valid_file_size(int);

#endif
