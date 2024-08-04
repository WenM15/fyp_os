#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "typedef.h"

struct cpu;

struct spinlock
{
	uint32 locked;
	char *name;
	struct cpu *p_cpu_struct;
};

#endif
