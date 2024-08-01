#include "typedef.h"

// sets n bytes of memory with a 8-bit value
void* memset(void *p_dest, uint8 value, uint64 n_bytes)
{
	uint8* p_byte = (uint8 *)p_dest;

	uint64 i = 0;
	while (i < n_bytes)
	{
		p_byte[i] = value;
		++i;
	}

	return p_dest;
}
