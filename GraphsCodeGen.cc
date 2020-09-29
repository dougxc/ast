/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: NodeCodeGen.cpp
//Author: Doug Simon
//Purpose: provides the operations to generate code for a CFG node that has been
//		analysed by the structuring algorithms.

#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include "Graphs.h"
#include "TypeDefs.h"
#include "StringFunctions.h"
#include "Options.h"
#include "MemAdvise.h"
#include "Stats.h"

extern Options options;

#define MAX_STRINGS_PER_BLOCK 5		// the maximum number of codestrings any one node
									// will add to the array of codestrings

//*********************************************************************
// Returns true if the stmt is a goto to the given label. Otherwise
// return false and update the valid label map for the given label
//*********************************************************************
bool GotoMatchesLabel(const char* stmt, const char* label)
{
	assert(label && label[0] == 'L' && label[strlen(label) - 2] == ':');
	char* gotoStmt = new char[strlen("goto ") + strlen(label)];
	strcpy(gotoStmt,"goto ");
	strncat(gotoStmt,label,strlen(label) - 2);

	return(static_cast<bool>(strstr(stmt,gotoStmt)));
}

//*********************************************************************
// Returns true if the given line is a closing bracket and nothing else
//*********************************************************************
bool IsCloseBracket(const char* line)
{
	int len = strlen(line);
	return(len >= 2 && line[len - 2] == '}' && line[len - 1] == '\n');
}

//*********************************************************************
// Post process the code to gemove unecessary goto's
//*********************************************************************
void RemGotos(StrArr &HLLCode, int maxLabel)
{
	// define an array of pointer to the goto labels
	DynArr<char*> GotoLabels;
	GotoLabels.Init(maxLabel);

	// define a map from a label to whether or not there is a 'goto' to it remaining
	bool* ValidLabel = new bool[maxLabel + 1];
	for (int i = 0; i < maxLabel + 1; i++)
		ValidLabel[i] = false;

	// work up through the generated code removing unecessary goto statements
	char* curLabel = new char[static_cast<int>(ceil(log10(maxLabel))) + 3];
	strcpy(curLabel,"");

	for (int i = HLLCode.Size() - 1; i != 0; i--)
	{
		char* curLine = HLLCode[i];
		if (curLine)
		{
			if (curLine[0] == 'L' && curLine[strlen(curLine) - 2] == ':')
			{
				// add this label to the array of label pointers
				GotoLabels.Add(curLine);
	
				strcpy(curLabel,curLine);
			}
			// if this is a goto statement then either remove it or update the 
			// valid label map
			else if (strstr(curLine,"goto L"))
			{
				if (strcmp(curLabel,"") != 0 && GotoMatchesLabel(curLine,curLabel))
					strcpy(curLine,"");
				else
					ValidLabel[atoi(digits(curLine))] = true;
			}
			else if (strcmp(curLabel,"") != 0 && !IsCloseBracket(curLine))
				strcpy(curLabel,"");
		}
	}

	// work through the labels within the code, removing those that don't have a goto
	// to them anymore
	for (int i = 0; i < GotoLabels.Size(); i++)
	{
		curLabel = GotoLabels[i];
		if (!ValidLabel[atoi(digits(curLabel))])
			strcpy(curLabel,"");
	}
	delete[] ValidLabel;
}

//*********************************************************************
// Write the code in the data structure to the file
//*********************************************************************
void CodeToFile(StrArr &HLLCode, ofstream &outFile)
{
	for (int i = 0; i < HLLCode.Size(); i++)
		if (strcmp(HLLCode[i],"\0") != 0)
			outFile << HLLCode[i]; 
}

//*********************************************************************
// Procedure to generate code for each procedure
//*********************************************************************
void Graphs::CodeGen(char* fname)
{
#ifdef GETSTATS
	double t[3] = {0,0,0};
	dtime(t);
#endif

	ofstream outFile(concatstr(fname,".hll"));
	ProcHeader* curProc;

	//make sure file was successfully opened
	if (!outFile)
	{
		cerr << "Error: could not open output file." << endl;
		exit(1);
	}

	StrArr HLLCode;
	NodePtrArr followSet;
	NodePtrArr gotoSet;

	// generate code for each procedure
	for (curProc = procs; curProc; curProc= curProc->next)
	{
		// write out procedure header
		outFile << "\n" << curProc->name << "()\n{" << endl;

		// Allocate enough space for the HLL code.
		HLLCode.Init(curProc->size * MAX_STRINGS_PER_BLOCK);
	
		// write out the body of each procedure
		curProc->cfg->WriteCode(HLLCode, 1, NULL, followSet, gotoSet);
#ifdef CODEGEN
	if (options.genCode)
		for (int i = 0; i < curProc->size; i++)
			if (curProc->Ordering[i]->Traversed() != DFS_CODEGEN && curProc->Ordering[i]->GetType() != ret)
			{
			cerr << fname << ": Code was not generated for " << curProc->name;
				cerr << "(" << curProc->size << ")->" << i << "(" << curProc->Ordering[i]->Ident() << ")" << endl;
			}
#endif

#if 0	
		// Remove the unecessary goto's from the output
		if (options.removeGotos)
			RemGotos(HLLCode,curProc->size);
#endif

		// Send the generated code to the outFile
		CodeToFile(HLLCode, outFile);

		// write out procedure tail
		outFile << "}" << endl;	

		// clear the code array
		//for (int i = 0; i < HLLCode.Size(); i++)
		//	if (HLLCode[i])
		//		delete HLLCode[i];
	}
	
	// close the file
	outFile.close();

#ifdef GETSTATS
	dtime(t);
	stats.codeGenTime = t[1];
#endif

}
