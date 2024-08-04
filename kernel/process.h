#ifndef PROCESS_H
#define PROCESS_H

#include "typedef.h"
#include "spinlock.h"

struct cpu
{
	// a reference count that indicates the number of disable-interrupt request
	uint8 count_intr_off;

	// the state of SIE (Supervisor Interrupt Enable) before
	// the first access to a critical section
       	uint8 state_intr_bfr_csec;
};

enum process_state
{
	UNUSED,
	USED,
	SLEEPING,
	RUNNABLE,
	RUNNING,
	ZOMBIE
};

struct process
{
	struct spinlock lock;

	// p_process->lock must be held when using these variables
	enum process_state state;
	
	// p_process->lock need not be held as these variables are private to the process
	uint64 kern_stack; // virtual address of kernel stack
};

#endif
