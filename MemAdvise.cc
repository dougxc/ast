/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: MemAdvise.cpp
// Author: Doug Simon
// Purpose: gather memory usage statistics

#include "MemAdvise.h"
#include <malloc.h>

void* operator new(size_t sz)
{
	MemStats[0] += sz;
	MemStats[2] += sz;
	void* p = malloc(sz + sizeof(int));
	*((int*) p) = sz;
	return (void*)((int*)p + 1);
}

void operator delete(void* p)
{
	// removing this check leads to problems
	if(p != 0x0)
	{	
		MemStats[1] += *((int*)((int*)p - 1));
		MemStats[2] -= *((int*)((int*)p - 1));
		free ((int*)p - 1);
	}
}
