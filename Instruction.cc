/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>
#include "StringFunctions.h"
#include "Instruction.h"

//opcode returns opcode part of a line of assembly
//pre: line = <opcode> .*  where opcode = [a-z,]*
//post: <opcode> is returned
char *getOpcode(char *line)
{
	int i;
	char *opStr;

        //calculate size of opcode string
        for(i = 0;isalpha(line[i]) || line[i] == ','; i++);

        opStr = substr(line,0,i);
	return opStr;
}

void Instruction::copy (Instruction const &other)
{
	srep = mystrdup(other.srep);
	opcode = other.opcode;
	labels = other.labels;
	procLabel = mystrdup(other.procLabel);

	bTarget = other.bTarget;
	jTargets = other.jTargets;
	branchDestLabel = mystrdup(other.branchDestLabel);
	jmpDestLabels = other.jmpDestLabels;
}

void Instruction::destroy()
{
	if (srep) delete srep;
	if (procLabel) delete procLabel;
	if (branchDestLabel) delete branchDestLabel;
}

//default constructor
Instruction::Instruction()
	: srep(NULL), procLabel(NULL), bTarget(NULL), branchDestLabel(NULL)
{}

//copy constructor
Instruction::Instruction(Instruction const& other)
{
	//unconditionally copy other's data
	copy (other);
}

//destructor
Instruction::~Instruction ()
{
	//unconditionally deallocate
	destroy ();
}

//overloaded assignment
Instruction const &Instruction::operator= (Instruction const &other)
{
	//only take action if no auto-assignment
	if (this != &other)
	{
		destroy ();
		copy(other);
	}

	//return (reference to) current object for
	//chain assignments
	return (*this);
}

bool Instruction::operator==(Instruction const& other) const { return &other == this; }
bool Instruction::operator!=(Instruction const& other) const { return &other != this; }

void Instruction::InitString(char const *line)
{
        int i,j;
	char *opString;

	//build the srep
        for (i = 0; !isalpha(line[i]); i++);
	srep = substr(line,i,strlen(line) - i);

	//extract the opcode out of the srep
	opString = getOpcode(srep);
	opcode = String2Type(opString);

	//build the dest label(s) if this is a jmp or branch instruction
	if (iType2bbType(opcode) == cBranch || iType2bbType(opcode) == uBranch)
	{
		//find the position of the branch label in the instruction string
		sscanf(srep,"%*s%*[ ]%n%*s%n",&i,&j);

		//extract the branch label from the instruction
		branchDestLabel = substr(srep,i,j-i);
	}
	else if (opcode == iJmp)	//its a jmp instruction
	{
		char* sCopy = mystrdup(srep);	//need to copy srep as strtok modifies it string arg.
		char* aLabel;			//the labels extracted

		//tokenize the space seperated srep, adding all but the first token (the opcode)
		//to the array of jmp dest labels for this jmp instruction
		strtok(sCopy," ");
		aLabel = strtok(NULL," ");
		while (aLabel) {
			jmpDestLabels.Add(aLabel);
			aLabel = strtok(NULL, " ");
		}
	}
}

char const* Instruction::GetString() const { return (char const *)srep; }

iType Instruction::GetType() const { return opcode; }

void Instruction::AddLabel(char const* l)
{
	//is it a procedure label?
	if (l[0] != '.')
		procLabel = mystrdup(l);
	else
		labels.Add(mystrdup(l));
}

char const* Instruction::BranchDestLabel() const { return (char const*)branchDestLabel; }
const StrArr &Instruction::JmpDestLabels() const { return jmpDestLabels; }

bool Instruction::IsLabelled() const
{
	return (labels.Size() == 0 && procLabel == NULL);
}

bool Instruction::EndBlock() const
{
	switch (iType2bbType(opcode)) {
	case nway:
	case cBranch:
	case call:
	case uBranch:
	case ret:
		return true;
	default:
		return false;
	}
}

char const* Instruction::GetProcLabel() const { return (char const*)procLabel; }

const StrArr &Instruction::GetNonProcLabels() const { return labels; }

void Instruction::SetBranchDest(Instruction const &ins) { bTarget = (Instruction*) &ins; }

Instruction const* Instruction::GetBranchDest() const { return bTarget; }
	
void Instruction::AddJmpDest(Instruction const &ins)
{
	int i;

	//test whether or not ins is already in the list
	for (i = 0; i < jTargets.Size(); i++)
		if (jTargets[i] == &ins)
			return;

	jTargets.Add((Instruction*)&ins);	
}

const InsPtrArr &Instruction::GetJmpDests() const { return jTargets; }
