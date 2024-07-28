#include "typedef.h"
#include "process.h"
#include "parameters.h"
#include "riscv_register.h"

struct cpu cpu_storage[MAX_CPU];

uint64 r_cpuid()
{
	return r_tp();
}

struct cpu* which_cpu()
{
	uint64 cpu_id = r_cpuid();
	struct cpu *p_cpu_struct = &cpu_storage[cpu_id];
	return p_cpu_struct;
}
