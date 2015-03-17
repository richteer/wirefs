#ifndef U_FILE
#define U_FILE

#define MAX_FILE_NAME_SIZE 15

typedef struct file_struct_s {
	int inode_number;
	char file_name[MAX_FILE_NAME_SIZE+1];
	bool free;
} file_struct;

bool valid_file_size(int);

#endif
