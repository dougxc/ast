/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: instructions.h
//   By Doug Simon
//   Purpose: - provides a type for Sparc8 assembly instruction opcodes as well as 

#ifndef _INSTRUCTION_
#define _INSTRUCTION_

#include "TypeDefs.h"
#include "DynArr.h"

//forward declare the Instruction class so that the following typedefs will compile
class Instruction;

//define suitable type names for the instantiations of the DynArr template.
typedef DynArr<char*> StrArr;			//a dynamic length array of strings
typedef DynArr<Instruction*> InsPtrArr;	//a dynamic length array of Instruction pointers

class  Instruction {
public:	

	//default constructor function
	Instruction();			

	//copy constructor
	Instruction(Instruction const& other);	

	//destructor
	~Instruction();			

	//copy constructor
	Instruction const &operator=(Instruction const &other);	

	//compares instructions for equality
	bool operator==(Instruction const& other) const;	

	//compares instructions for inequality
   bool operator!=(Instruction const& other) const;	

	//builds most of the data stored in an instruction object
	//from the given string representation of the instruction.
	void InitString(char const*line);	

	//return the string representation of an instruction
	char const* GetString() const;	

	//returns the opcode
	iType GetType() const;		

	//add l to the labels for this instruction
	void AddLabel(char const *l);		

	//get the label for the dest instruction of a branch instruction
	char const* BranchDestLabel() const;	

	//get the array of jmp dest labels of a jmp instruction
	const StrArr &JmpDestLabels() const;	

	//are there any labels at this instruction?
	bool IsLabelled() const;	

	//does this instruction end a basic block?
	bool EndBlock() const;		

	//the procedure label at this instruction (if any)
	const char* GetProcLabel() const;	

	//the non-procedure labels at this ins. (if any)
	const StrArr &GetNonProcLabels() const;

	//pre: this is a branch instruction
	//set the branch destination
	void SetBranchDest(Instruction const &ins); 

	//pre: this is a branch instruction
	//return the branch destination
	Instruction const* GetBranchDest() const;

	//pre: this is a jump instruction
	//add a pointer to a jump destination	
	void AddJmpDest(Instruction const &ins);

	//pre: this is a jump instruction
	//return the list of pointers to jump destinations
	const InsPtrArr &GetJmpDests() const;

private:
	char *srep;			//string representation of the instruction
	iType opcode;			//opcode of instruction
	StrArr labels;			//labels at this instruction
	char* procLabel;

	Instruction* bTarget;		//target instruction of a branch
	InsPtrArr jTargets;		//target instructions of a jump
	char* branchDestLabel;		//branch dest label
	StrArr jmpDestLabels;		//jmp dest labels 

	//two primitve (i.e. unconditional) copy and destroy functions
	void copy (Instruction const& other);
	void destroy(void);
};

#endif
