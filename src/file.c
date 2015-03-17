#include <stdbool.h>
#include <time.h>
#include "userfs.h"
#include "inode.h"

bool valid_file_size(int size) {
	return size <= MAX_BLOCKS_PER_FILE;
}
