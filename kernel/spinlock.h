#include "typedef.h"
#include "process.h"

#ifndef SPINLOCK_H
#define SPINLOCK_H

struct spinlock
{
	uint32 locked;
	struct cpu *p_cpu_struct;
};

#endif
