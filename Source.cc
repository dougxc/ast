/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

#include <fstream.h>
#include <stdlib.h>
#include "StringFunctions.h"
#include "Source.h"

#ifdef GETSTATS
#include "Stats.h"
#endif

//*****************************************************************************
//Unfortunately I need this struct and it's ordering function declared globally
//as I cannot get qsort and bsearch to work on it when declared inside LabelArr
//*****************************************************************************
struct _label {
	char* name;
	int idx;
	~_label() { if (name) delete name; }
};

int cmp(const void* l1, const void* l2) 
	{ return strcmp(((_label*)l1)->name,((_label*)l2)->name);}


//*******************************************************************************
//We need an auxillary class to store an array of <label,instruction_index> pairs
//*******************************************************************************
class LabelArr {
public:
	LabelArr(int s);		//constructor that allocates space for s elements
	~LabelArr() { if (pos) delete[] lArr; }
	void Add(char const* l, int idx);	//add the <l,idx> pair to the array
	void Sort();			//sort the internal array for faster lookups
	int Find(char const* l);		//find the index of the instruction at which l occurs
private:
	_label* lArr;
	int pos;
};

LabelArr::LabelArr(int s) { lArr = new _label[s]; pos = 0; }
void LabelArr::Add(char const* l, int idx) { lArr[pos].name = mystrdup(l); lArr[pos++].idx = idx; }
void LabelArr::Sort() { qsort(lArr,pos,sizeof(_label),cmp); }
int  LabelArr::Find(char const* l)
{
#ifdef DEBUG
	if (!l)	{
		cerr << "Error: About to search for an empty label." << endl;
		exit(1);
	}
#endif

	_label tmp, *ptr;
	tmp.name = mystrdup(l);
	ptr = (_label*) bsearch(&tmp,lArr,pos,sizeof(_label),cmp);
#ifdef DEBUG
	if (!ptr) {
		cerr << "Error: label " << l << " was not found." << endl;
		exit(1);
	}
#endif
	return ptr->idx;
}

static bool IsLabel(char* line);

//**********************************************
//Implementation of the Source class begins here
//**********************************************

void Source::Build(char *fname)
{
	ifstream inFile(fname);		//input file stream is initialised to the file denoted by fname
	
	char* line;		//line of source code
	int size, lineSize;	//stores various sizes when needed
	int insIdx = 0;		//index into array of instructions

	//check to make sure file was opened properly
	if (!inFile) {
		cerr << "Error: file \"" << fname << "\" was not opened successfully." << endl;
		exit (1);
	}

	//the first line of fname will contain the number of instructions
	//in the file. Use this to reallocate space for the array of instructions.
	inFile >> size;
	arr.Init(size, true);
#ifdef GETSTATS
	stats.numAsmIns = size;
#endif

	//the second line in fname will contain the number of labels
	//in the file. Use this to allocate space for the array of labels.
	inFile >> size;
	LabelArr labels(size);

	//the third line in fname will tell us the maximum length of
	//any line of input. Use this to allocate space for the string variable
	//used to read in each line. 
	inFile >> lineSize;
	line = new char[lineSize];
	
	//Ignore up to the beginning of the next line
	inFile.ignore(lineSize,'\n');

	//process each line of the source file
	while (inFile.getline(line,lineSize))
	{
		if (IsLabel(line))
		{
			//add an entry to the array of labels
			labels.Add(line, insIdx);

			//add the label to the list of labels for the next instruction
			arr[insIdx].AddLabel(line);
		}
		else //it's an instruction line
			arr[insIdx++].InitString(line);
	}

	//sort the array of labels
	labels.Sort();

	//iterate through the array of instructions, filling in the control
	//flow information where necessary
	for (insIdx = 0; insIdx < arr.Size(); insIdx++)
	{
		char const* branchDest = arr[insIdx].BranchDestLabel();	//dest label of a branch instruction
		const StrArr& jmpDestLabels = arr[insIdx].JmpDestLabels();		//dest labels of a jmp instruction
#ifdef DEBUG
		if (branchDest && jmpDestLabels.Size() > 0) {
			cerr << "Error: an ins. has both a branch and jmp labels" << endl;
			exit(1);
		}
#endif
		if (branchDest)
		{
			Instruction& refIns = arr[labels.Find(branchDest)];
			arr[insIdx].SetBranchDest(refIns);
		}

		if (jmpDestLabels.Size() > 0)
			for (int i = 0; i < jmpDestLabels.Size(); i++)
			{
				Instruction& refIns = arr[labels.Find(jmpDestLabels[i])];
#ifdef DEBUG
				cerr << "Adding jmp out edge to instruction ";
				cerr << ((int)&refIns - (int)&arr[0]) / sizeof(Instruction) << endl;
#endif
				arr[insIdx].AddJmpDest(refIns);
			}
	}	

#ifdef TESTSOURCE
	for (insIdx = 0; insIdx < arr.Size(); insIdx++)
	{
		InsPtrArr iArr;
		StrArr nonPLabels;

		cerr << insIdx << '\t';

		if (arr[insIdx].GetProcLabel())
			 cerr << arr[insIdx].GetProcLabel() << "\t";
		nonPLabels = arr[insIdx].GetNonProcLabels();
		for (int i = 0; i < nonPLabels.Size(); i++)
			cerr << nonPLabels[i];
		cerr << "\t";

		cerr << arr[insIdx].GetString();
		if (arr[insIdx].GetBranchDest())
			 cerr << (arr[insIdx].GetBranchDest())->GetString();

		iArr = arr[insIdx].GetJmpDests();
		if (iArr.Size() > 0) {
			cerr << "  {";
			for (int i = 0; i < iArr.Size(); i++)
				if (i < iArr.Size() - 1)
					cerr << ((int)iArr[i] - (int)&arr[0]) / sizeof(Instruction) << ", ";
				else
					cerr << ((int)iArr[i] - (int)&arr[0]) / sizeof(Instruction) << "}";
		}
		cerr<< endl;
	}	

	inFile.close();
#endif
}

int Source::Size() const { return arr.Size(); }
Instruction &Source::operator[](int i) const { return arr[i]; }

//returns whether or not the given line is a label.
//A line is a label if it ends in a semicolon. Strips the trailing ':' off
//the line if it is a label
static bool IsLabel(char *line)
{
	if (line[strlen(line) - 1] == ':')
	{
		line[strlen(line) - 1] = '\0';
		return true;
	}
	else
		return false;
}
