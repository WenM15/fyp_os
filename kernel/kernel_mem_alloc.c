#include "spinlock.h"
#include "typedef.h"
#include "pages.h"
#include "decl.h"
#include "qemu_memlayout.h"

extern uint8* end_kerncode;

// contains the next physical page's address
struct phypage_node
{
	struct phypage_node *next;
};

// whenever the freelist needs to be access, this handle facilitates the access
// the freelist is a list of free physical pages
struct
{
	struct spinlock lock;
	struct phypage_node *freelist;
} kernel_phypage_handle;

void kernel_phypage_init()
{
	init_lock(&kernel_phypage_handle.lock);
	free_phypage_range(end_kerncode, (void*)END_KERNMEM);
}

// specifies the range of physical memory to be freed.
// if lower range is not aligned, take the next aligned page.
// upper range is included in the page to be freed.
void free_phypage_range(void *pa_start, void *pa_end)
{
	uint8 *pa = (uint8*)PAGE_ROUND_UP((uint64)pa_start);
	while (pa + PAGE_SIZE <= (uint8*)pa_end)
	{
		free_kernpage(pa);
		pa += PAGE_SIZE;
	}
}

// adds a physical page to the freelist
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

// obtain a physical page from the freelist
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
