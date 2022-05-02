#ifndef _CT_STL_ALLOC_H
#define _CT_STL_ALLOC_H

#include "stdlib.h"
#include "types.h"

typedef struct {
	void* mem;
	const u64 size;
} Blk;

Blk alloc_blk(const register u64 _size)
{
	Blk blk = {
		.mem = malloc(_size),
		.size = _size
	};
	return blk;
}

Blk calloc_blk(const register u64 _amount, const register u64 _size)
{
	Blk blk = {
		.mem = calloc(_amount, _size),
		.size = (_amount*_size)
	};
	return blk;
}

void free_blk(const Blk* restrict _blk)
{
	free(_blk->mem);
	return;
}

#endif // End CT_STL_ALLOC_H
