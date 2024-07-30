#include "typedef.h"

#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

void kernel_vm_init();
uint64* create_kern_pgtable();
void kern_add_map(uint64* p_kern_pgtable, uint64 va, uint64 pa, uint64 size, uint16 perm);
uint8 map_pages(uint64* p_pgtable, uint64 va, uint64 pa, uint64 size, uint16 perm);
uint64* walk(uint64* p_pgtable, uint64 va, uint8 alloc);

#endif
