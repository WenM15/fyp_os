#ifndef KERNEL_MEM_ALLOC_H
#define KERNEL_MEM_ALLOC_H

void kernel_phypage_init();
void free_phypage_range(void *pa_start, void *pa_end);
void free_kernpage(void *pa);
void* alloc_kernpage();

#endif
