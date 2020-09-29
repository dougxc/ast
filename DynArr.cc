/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

#ifndef _DYNARR_IMPL_
#define _DYNARR_IMPL_
#pragma implementation

#include <string.h>
#include <iostream.h>
#include <assert.h>
#include "DynArr.h"


//copy (): unconditionally copy other object's data
template <class T>
/*inline*/ void DynArr<T>::copy (DynArr<T> const &other)
{
	sz = other.sz;
	avail = other.avail;
	dynArr = new T[other.sz];

	memcpy(dynArr,other.dynArr,sizeof(T) * sz);
}

//destroy(): unconditionally deallocate data
template <class T>
/*inline*/ void DynArr<T>::destroy()
{
	//only destruct the array if it is non-empty
	if (sz + avail > 0)
		delete[] dynArr;
}

template <class T>
/*inline*/ DynArr<T>::DynArr() :
	dynArr(0), sz(0), avail(0)
{}

template <class T>
/*inline*/ DynArr<T>::DynArr(int s) :
	sz(0),avail(s)
{
	dynArr = new T[avail];
}

template <class T>
/*inline*/ DynArr<T>::DynArr(const DynArr<T> &other)
{
	copy(other);
}

template <class T>
/*inline*/ DynArr<T>::~DynArr()
{
	destroy();
}

template <class T>
bool DynArr<T>::IsIn(T const elem) const
{
	for (int i = 0; i < sz; i++)
		if (dynArr[i] == elem)
			return true;
	return false;
}

template <class T>
DynArr<T> & DynArr<T>::operator =(const DynArr<T> &other)
{
	//only take action if no auto-assignment
	if (this != &other)
	{
		destroy();
		copy(other);
	}

	//return (reference to) current object for chain assignments
	return (*this);
}

template <class T>
/*inline*/ T& DynArr<T>::operator[] (int idx) const
{
	assert(idx >= 0 && idx <= (sz + avail - 1 ));
	return dynArr[idx];
}

template <class T>
void DynArr<T>::Add(T const elem)
{
	//if still available space, then just add new element at the end
	if (avail > 0)
	{
		dynArr[sz++] = elem;
		avail--;
	}
	else
	{
		T* newArr = new T[sz + 6];

		if (sz > 0)
			memcpy(newArr,dynArr,sizeof(T) * (sz));

		newArr[sz] = elem;

		if (sz > 0)
			delete[] dynArr;

		sz++;
		dynArr = newArr;
		avail = 5;
	}
}

template <class T>
void DynArr<T>::Remove(T const elem)
{
	assert(IsIn(elem));

	if (--sz != 0)
	{	
		T* newArr = new T[sz + 5];
		int newPos = 0;
	
		for (int i = 0; i < (sz + 1); i++)
			if (dynArr[i] != elem)
				newArr[newPos++] = dynArr[i];

		delete[] dynArr;
		dynArr = newArr;	
		avail = 5;
	}
	else
	{
		avail++;
	}
}

template <class T>
void DynArr<T>::RemoveLast()
{
	assert(sz > 0);
	sz--;
	avail++;
}

template <class T>
void DynArr<T>::RemoveFirst()
{
	assert(sz > 0);
	if (--sz != 0)
	{
		T* newArr = new T[avail+sz + 1];
		memcpy(newArr,&(dynArr[1]),sizeof(T)*(sz));
		delete[] dynArr;
		dynArr = newArr;
	}
	avail++;
}

template <class T>
/*inline*/ int DynArr<T>::Size() const { return sz; }

template <class T>
/*inline*/ void DynArr<T>::Init(int s, bool setSize = false)
{
	dynArr = new T[(avail = s)];
	if (setSize)
		sz = s;
	else
		sz = 0;
}


// need to instantiate all the templates that we use

class CFGNode;
class DerivedGraph;
class Instruction;
template class DynArr<CFGNode *>;
template class DynArr<DerivedGraph *>;
template class DynArr<Instruction *>;
template class DynArr<char *>;
#include "Instruction.h"
template class DynArr<Instruction>;

#endif /* _DYNARR_IMPL_ */
