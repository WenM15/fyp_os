#include "typedef.h"

#ifndef MEMORY_H
#define MEMORY_H

// Pages
#define PAGE_SIZE 4096

#define PAGE_ROUND_UP(pa) (((pa)+PAGE_SIZE-1) & ~(PAGE_SIZE-1))

// Memory operation
void* memset(void *p_dest, uint8 value, uint64 n_bytes);

#endif
