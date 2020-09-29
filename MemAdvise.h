/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: MemAdvise.h
// Author: Doug Simon
// Purpose: gather memory usage statistics

#ifndef _MEMADVISE_
#define _MEMADVISE_

#include <malloc.h>

extern int MemStats[3]; // Total allocated, total deallocated, currently
						// allocated

// built in operator new. Updates the global memory stats variable
void* operator new(size_t sz);

// built in operator delete. Updates the global memory stats variable
void operator delete(void* p);
#endif
