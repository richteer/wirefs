#ifndef U_BITMAP
#define U_BITMAP

#include "blocks.h"

#define BIT_FIELD unsigned
#define BIT_MAP_BLOCK 1
#define BIT_MAP_SIZE (BLOCK_SIZE_BYTES/sizeof(BIT_FIELD))
#define BITS_PER_FIELD (sizeof(unsigned) * 8)

extern BIT_FIELD bit_map[BIT_MAP_SIZE];

void init_bit_map();
void write_bitmap();

#endif

