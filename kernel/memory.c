#include "typedef.h"

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
