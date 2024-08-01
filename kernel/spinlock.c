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

// disable interrupts but adds a reference count
// if there are two requests to disable interrupts,
// then the reference count is two.
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

// enable interrupts, and decrement the reference count
// if the reference count is two,
// and if there is one request to enable interrupt,
// then interrupt won't be enabled because
// reference count is still one.
//
// interrupt is enabled when reference count hits zero
// and the previous interrupt state is enabled.
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

// checks whether the current CPU is holding the lock
uint8 holding(struct spinlock* p_lock)
{
	uint8 held = (p_lock->locked && p_lock->p_cpu_struct == which_cpu());
	return held;
}

// acquires a lock for the current CPU
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

// release a lock for the current CPU
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


