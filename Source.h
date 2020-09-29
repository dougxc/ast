/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: source.h
//Author: Doug Simon
//Purpose: provides a parser for an assembly program*. Constructs an array of
//	instruction classes. 
//
//	*the program parsed must have been preprocessed to remove any assembler
//	directives or comments. A preprocessor wriiten for this purpose is
//	provided with the package of which this parser is a part

#ifndef _SOURCECLASS_
#define _SOURCECLASS_

#include "Instruction.h"
#include "DynArr.h"

//Simplying typedefs
typedef DynArr<Instruction> InsArr;

class Source {
public:
	void Build(char* fname);	//build the array of instructions from the file denoted by fname
	int Size() const;						//number of instructions
	Instruction &operator[](int i) const;	//return a reference to the i'th instruction
private:
	InsArr arr;
};

#endif
