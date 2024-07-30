#include "typedef.h"
#include "spinlock.h"
#include "riscv_register.h"
#include "process.h"
#include "decl.h"

void init_lock(struct spinlock *p_lock)
{
	p_lock->locked = 0;
	p_lock->p_cpu_struct = 0;
}

void increment_intr_off()
{
	uint8 prev_intr = intr_get();

	intr_off();
	
	struct cpu *p_cpu_struct = which_cpu();
	if (p_cpu_struct->count_intr_off == 0)
	{
		p_cpu_struct->state_intr_bfr_csec = prev_intr; 
	}
	p_cpu_struct->count_intr_off += 1;
}

void decrement_intr_off()
{
	struct cpu *p_cpu_struct = which_cpu();
	
	if (intr_get())
	{
		// panic
	}
	if (p_cpu_struct->count_intr_off < 1)
	{
		// panic
	}
	
	p_cpu_struct->count_intr_off -= 1;

	if (p_cpu_struct->count_intr_off == 0 && p_cpu_struct->state_intr_bfr_csec)
	{
		intr_on();
	}
}

uint8 holding(struct spinlock* p_lock)
{
	uint8 held = (p_lock->locked && p_lock->p_cpu_struct == which_cpu());
	return held;
}

void acquire(struct spinlock *p_lock)
{
	increment_intr_off();
	if (holding(p_lock))
	{
		// panic
	}
	
	while(atomic_swap(&p_lock->locked, 1) != 0){}
	
	asm volatile("fence" : : : "memory");

	p_lock->p_cpu_struct = which_cpu();
}

void release(struct spinlock *p_lock)
{
	if (!holding(p_lock))
	{
		// panic
	}

	p_lock->p_cpu_struct = 0;

	asm volatile("fence" : : : "memory");

	if (atomic_swap(&p_lock->locked, 0) != 1)
	{
		// panic
	}

	decrement_intr_off();
}


