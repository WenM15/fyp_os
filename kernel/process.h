#include "typedef.h"

#ifndef PROCESS_H
#define PROCESS_H

struct cpu
{
	uint8 count_intr_off;
       	uint8 state_intr_bfr_csec;
};

uint64 r_cpuid();
struct cpu* which_cpu();

#endif
