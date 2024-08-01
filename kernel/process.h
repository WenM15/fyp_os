#include "typedef.h"

#ifndef PROCESS_H
#define PROCESS_H

struct cpu
{
	// a reference count that indicates the number of disable-interrupt request
	uint8 count_intr_off;

	// the state of SIE (Supervisor Interrupt Enable) before
	// the first access to a critical section
       	uint8 state_intr_bfr_csec;
};

#endif
