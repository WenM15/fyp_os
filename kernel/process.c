#include "typedef.h"
#include "decl.h"
#include "process.h"
#include "parameters.h"
#include "riscv_register.h"
#include "pages.h"
#include "qemu_memlayout.h"
#include "spinlock.h"

struct cpu cpu_storage[MAX_CPU];

struct process process_storage[MAX_PROCESS];

uint32 next_pid = 1;
struct spinlock pid_lock;

struct spinlock wait_lock;

uint64 r_cpuid()
{
	return r_tp();
}

// returns the pointer to the current CPU's structure
struct cpu* which_cpu()
{
	uint64 cpu_id = r_cpuid();
	struct cpu *p_cpu_struct = &cpu_storage[cpu_id];
	return p_cpu_struct;
}

// add mapping for every process's kernel stack
void kernstack_add_map(uint64* p_pgtable)
{
	struct process *p_process = process_storage;

	while (p_process < process_storage + MAX_PROCESS)
	{
		uint8 *pa = alloc_kernpage();
		if (pa == 0)
		{
			// panic
		}
		uint64 va = KERN_STACK((uint32)(p_process - process_storage));
		kern_add_map(p_pgtable, va, (uint64)pa, PAGE_SIZE, PTE_R | PTE_W);

		++p_process;
	}
}

void process_init()
{
	struct process *p_process = process_storage;

	init_lock(&pid_lock, "pid_lock");
	init_lock(&wait_lock, "wait_lock");

	while (p_process < process_storage + MAX_PROCESS)
	{
		init_lock(&p_process->lock, "process");
		p_process->state = UNUSED;
		p_process->kern_stack = KERN_STACK((uint32)(p_process - process_storage));
		++p_process;
	}
}
