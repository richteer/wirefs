#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "userfs.h"
#include "crash.h"
#include "sb.h"
#include "blocks.h"
#include "bitmap.h"

#define BPF BITS_PER_FIELD

//hardinc modified
void allocate_block(int blockNum)
{
	int bit = blockNum & (BPF - 1);
	int ind = blockNum / BPF;

	assert(blockNum < BIT_MAP_SIZE * BPF);
	bit_map[ind] |= 1 << bit;
}

//hardinc modified
void free_block(int blockNum)
{
	int bit = blockNum & (BPF - 1);
	int ind = blockNum / BPF;

	assert(blockNum < BIT_MAP_SIZE);
	bit_map[ind] &= ~(1 << bit);
}

//hardinc modified
int find_free_block() {
	int i,j;
	
	// search for bit field with clear bit
	for(i=0;i < sb.disk_size_blocks; i++) {
		if(bit_map[i] != ~0) {
			// search for clear bit in bit field
			for (j = 0; j < BPF; j++) {
				if (~bit_map[i] & (1 << j)) {
					return i * BPF + j;
				}
			}
			assert(0);
		}
	}
	return -1;
}

void write_block(DISK_LBA block, const void * data, int size) {
	write_block_offset(block, data, size, 0);
}

void write_block_offset(DISK_LBA block, const void * data, int size, int offset) {
	lseek(virtual_disk, BLOCK_SIZE_BYTES * block + offset, SEEK_SET);
	crash_write(virtual_disk, data, size);
}

void read_block(DISK_LBA block, void * data, int size) {
	lseek(virtual_disk, BLOCK_SIZE_BYTES * block, SEEK_SET);
	read(virtual_disk, data, size);
}
