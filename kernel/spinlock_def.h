#include "typedef.h"
#include "process.h"

#ifndef SPINLOCK_H
#define SPINLOCK_H

struct spinlock
{
	uint32 locked;
	struct cpu *p_cpu_struct;
};

void init_lock(struct spinlock* p_lock);
void increment_intr_off();
void decrement_intr_off();
uint8 holding(struct spinlock* p_lock);
void acquire(struct spinlock *p_lock);
void release(struct spinlock *p_lock);

#endif
