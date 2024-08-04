#include "typedef.h"
#include "pages.h"
#include "decl.h"
#include "qemu_memlayout.h"
#include "riscv_register.h"

extern uint8* end_text;
extern uint8* trampoline;

// the kernel's root pagetable's memory address.
// the pagetable has the size of a page (4096 bytes).
uint64* p_kern_pgtable;

void kernel_vm_init()
{
	p_kern_pgtable = create_kern_pgtable();
}

// creates the kernel's page table and
// maps specified virtual addresses to physical addresses.
uint64* create_kern_pgtable()
{
	uint64* p_kern_pgtable;
	
	p_kern_pgtable = (uint64*)alloc_kernpage();
	memset(p_kern_pgtable, 0, PAGE_SIZE);
	
	kern_add_map(p_kern_pgtable, KERN_BASE, KERN_BASE, (uint64)end_text-KERN_BASE, PTE_R | PTE_X);

	kern_add_map(p_kern_pgtable, (uint64)end_text, (uint64)end_text, END_KERNMEM-(uint64)end_text, PTE_R | PTE_W);

	// allocate and map a kernel stack for each process
	kernstack_add_map(p_kern_pgtable);	

	return p_kern_pgtable;
}

// add kernel's virtual addresses to physical addresses
void kern_add_map(uint64* p_kern_pgtable, uint64 va, uint64 pa, uint64 size, uint16 perm)
{
	if (map_pages(p_kern_pgtable, va, pa, size, perm) != 0)
	{
		// panic
	}
}

// set physical address to lowest level PTE and adds page permission.
// both starting and ending of virtual address
// passed in might not be aligned
// but both are included in the mapping.
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

// return the virtual address of the lowest level PTE
// corresponding to the virtual address passed into it.
// if alloc is 1, create any required pagetable or pages.
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
	
	// return the virtual address of lowest level PTE
	return p_pgtable + PAGE_INDEX(0, va);
}

void kernel_enable_paging()
{
	// ensure all prior memory writes that could depend on the
	// TLB are completed
	flush_TLB();

	w_satp(MAKE_SATP(p_kern_pgtable));

	// ensure new memory mappings are reflected
	// by flushing old translations
	flush_TLB();
}
