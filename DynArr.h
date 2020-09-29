/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: DynArr.h
//Author: Doug Simon
//Purpose: provide a template for a dynamic array of T that can:
//	i) be initialised with an abritary size
//	ii) have elements added at the end
//	iii) be indexed using the [] 
//
//	Type T must have the following operations defined: !=, ==, =
//	NOTE: The function definitions have been included in this file due to 
//		g++'s template instantiation characteristics.

#ifndef _DYNARR_
#define _DYNARR_

#pragma interface

#include <string.h>
#include <iostream.h>
#include <assert.h>

template <class T>
class DynArr {
public:

	//default constructor
	DynArr();			

	//copy constructor
	DynArr(const DynArr<T> &other);	

	//the size of the array is known
	DynArr(int s);		

	//destructor
	~DynArr ();			

	//copy constructor
	DynArr<T> & operator =(const DynArr<T>& other);

	//membership predicate
	bool IsIn(T const elem) const;
 
	//access by index
	T& operator[] (int idx) const;	

	//add elem to the array. NOTE: if elem is a pointer to memory
	//whose contents will change, send in a pointer to a copy of the contents.
	//Eg. when sending in a string whose contents will change, use strdup.
	void Add(T const elem);		

	//remove elem from the array. It must be in the array to begin with.
	void Remove(T const elem);	

	//remove the first element from the array. 
	void RemoveFirst();

	//remove the last element from the array. 
	void RemoveLast();

	//return the size of the array
	int Size() const;		

	//a delayed constructor type of function that 
	//will allocate space in the array for s entries.
	//If the elements will not be added explicity with
	//Add, then setSize should be set to true.
	// Bad design but will give better performance :-)
	void Init(int s, bool setSize = false);	
							
private:

	T* dynArr;
	int sz;
	int avail;

	//two primitve (i.e. unconditional) copy and destroy functions
        void copy (DynArr<T> const& other);
        void destroy(void);
};

#endif
