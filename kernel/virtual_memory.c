#include "typedef.h"
#include "memory.h"
#include "kernel_mem_alloc.h"
#include "virtual_memory.h"
#include "qemu_memlayout.h"

extern uint8* end_text;
extern uint8* trampoline;

uint64* p_kern_pgtable;

void kernel_vm_init()
{
	p_kern_pgtable = create_kern_pgtable();
}

uint64* create_kern_pgtable()
{
	uint64* p_kern_pgtable;
	
	p_kern_pgtable = (uint64*)alloc_kernpage();
	memset(p_kern_pgtable, 0, PAGE_SIZE);
	
	kern_add_map(p_kern_pgtable, KERN_BASE, KERN_BASE, (uint64)end_text-KERN_BASE, PTE_R | PTE_X);

	kern_add_map(p_kern_pgtable, (uint64)end_text, (uint64)end_text, END_KERNMEM-(uint64)end_text, PTE_R | PTE_W);

	return p_kern_pgtable;
}

void kern_add_map(uint64* p_kern_pgtable, uint64 va, uint64 pa, uint64 size, uint16 perm)
{
	if (map_pages(p_kern_pgtable, va, pa, size, perm) != 0)
	{
		// panic
	}
}

// set physical address to highest level PTE (third level)
uint8 map_pages(uint64* p_pgtable, uint64 va, uint64 pa, uint64 size, uint16 perm)
{
	uint64 va_aligned, va_aligned_last;
	uint64* p_pte;

	if (size == 0)
	{
		// panic
	}

	va_aligned = PAGE_ROUND_DOWN(va);
	va_aligned_last = PAGE_ROUND_DOWN(va + size - 1);

	while (1)
	{
		if ((p_pte = walk(p_pgtable, va_aligned, 1)) == 0)
		{
			return -1;
		}
		if (*p_pte & PTE_V)
		{
			// panic
		}
		*p_pte = PA2PTE(pa) | perm | PTE_V;
		if (va_aligned == va_aligned_last)
		{
			break;
		}
		
		va_aligned += PAGE_SIZE;
		pa += PAGE_SIZE;
	}

	return 0;
}


uint64* walk(uint64* p_pgtable, uint64 va, uint8 alloc)
{
	if (va >= MAX_VA)
	{
		// panic
	}

	for (uint8 level = 2; level > 0; --level)
	{
		uint64 *p_pte = p_pgtable + PAGE_INDEX(level, va);
		if (*p_pte & PTE_V)
		{
			p_pgtable = (uint64*)PTE2PA(*p_pte);
		}
		else
		{
			if (alloc && (p_pgtable = (uint64*)alloc_kernpage()))
			{
				memset(p_pgtable, 0, PAGE_SIZE);
				*p_pte = PA2PTE(p_pgtable) | PTE_V;
			}
			else
			{
				return 0;
			}
		}

	}

	return p_pgtable + PAGE_INDEX(0, va);
}
