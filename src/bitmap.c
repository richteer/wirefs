#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "userfs.h"
#include "crash.h"
#include "sb.h"
#include "blocks.h"
#include "bitmap.h"

BIT_FIELD bit_map[BIT_MAP_SIZE];

void init_bit_map() {
	DISK_LBA i;
	for (i = 0; i < BIT_MAP_SIZE; i++) {
		bit_map[i] = 0;
	}
}

void write_bitmap() {
	write_block(BIT_MAP_BLOCK, bit_map, sizeof(BIT_FIELD)*BIT_MAP_SIZE);
}
