#include "typedef.h"

#ifndef DECL_H
#define DECL_H

struct spinlock;

// process.c
uint64 r_cpuid();
struct cpu* which_cpu();

// spinlock.c
void init_lock(struct spinlock* p_lock);
void increment_intr_off();
void decrement_intr_off();
uint8 holding(struct spinlock* p_lock);
void acquire(struct spinlock *p_lock);
void release(struct spinlock *p_lock);

// memory.c
void* memset(void *p_dest, uint8 value, uint64 n_bytes);

// kernel_mem_alloc.c
void kernel_phypage_init();
void free_phypage_range(void *pa_start, void *pa_end);
void free_kernpage(void *pa);
void* alloc_kernpage();

// virtual_memory.c
void kernel_vm_init();
uint64* create_kern_pgtable();
void kern_add_map(uint64* p_kern_pgtable, uint64 va, uint64 pa, uint64 size, uint16 perm);
uint8 map_pages(uint64* p_pgtable, uint64 va, uint64 pa, uint64 size, uint16 perm);
uint64* walk(uint64* p_pgtable, uint64 va, uint8 alloc);

#endif
