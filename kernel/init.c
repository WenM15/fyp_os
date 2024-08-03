#include "decl.h"

void func_init()
{
	kernel_phypage_init();
	kernel_vm_init();
	kernel_enable_paging();
}
