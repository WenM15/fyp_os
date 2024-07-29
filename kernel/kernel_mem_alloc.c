#include "spinlock_def.h"
#include "typedef.h"
#include "memory.h"
#include "kernel_mem_alloc.h"
#include "qemu_memlayout.h"

extern uint8* end_kerncode;

struct phypage_node
{
	struct phypage_node *next;
};

struct
{
	struct spinlock lock;
	struct phypage_node *freelist;
} kernel_phypage_handle;

// function declaration
//void free_phypage_range(void *pa_start, void *pa_end);

void kernel_phypage_init()
{
	init_lock(&kernel_phypage_handle.lock);
	free_phypage_range(end_kerncode, (void*)END_KERNMEM);
}

void free_phypage_range(void *pa_start, void *pa_end)
{
	uint8 *pa = (uint8*)PAGE_ROUND_UP((uint64)pa_start);
	while (pa + PAGE_SIZE <= (uint8*)pa_end)
	{
		free_kernpage(pa);
		pa += PAGE_SIZE;
	}
}

void free_kernpage(void *pa)
{
	struct phypage_node* n;

	if (((uint64)pa % PAGE_SIZE) != 0 || (uint8*)pa < end_kerncode || (uint64)pa >= END_KERNMEM)
	{
		// panic
	}
	
	memset(pa, 1, PAGE_SIZE);

	n = (struct phypage_node*)pa;

	acquire(&kernel_phypage_handle.lock);
	// critical section
	n->next = kernel_phypage_handle.freelist;
	kernel_phypage_handle.freelist = n;
	// end of critical section
	release(&kernel_phypage_handle.lock);
}

void* alloc_kernpage()
{
	struct phypage_node *n;

	acquire(&kernel_phypage_handle.lock);
	// critical section
	n = kernel_phypage_handle.freelist;
	if (n)
	{
		kernel_phypage_handle.freelist = n->next;
	}
	// end of critical section
	release(&kernel_phypage_handle.lock);

	if (n)
	{
		memset((void*)n, 5, PAGE_SIZE);
	}

	return (void*)n;
}
