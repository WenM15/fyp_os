#include "kernel_mem_alloc.h"
#include "virtual_memory.h"

void func_init()
{
	kernel_phypage_init();
	kernel_vm_init();
}
