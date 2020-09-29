/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: GraphsPrint.cpp
//Author: Doug Simon
//Purpose: gives the implementation for the Graphs's operations that are common to both
//	algorithms

#include <fstream.h>
#include "StringFunctions.h"
#include "Options.h"

extern Options options;

//an array used to find the string representation of a structured type
char* StructString[5] = { "loop", "cond", "loopCond", "seq" };

//an array used to find the string representation of a loop type
char* LoopString[3] = { "preTest", "postTest", "endless" };

//an array used to find the string representation of a conditional type
char* CondString[4] = { "if-then", "if-then-else", "if-else", "case" };

void Graphs::GenerateGraphvizFile(char* fname)
{
	ofstream outFile(concatstr(fname,".dot"));	//	
	CFGNode* curNode;
	ProcHeader* curProc; 

	//make sure file was successfully opened
	if (!outFile)
	{
		cerr << "Error: could not open " << strcat(fname,".dot") << " for writing." << endl;
		exit(1);
	}

	//write header information to file
	outFile << "digraph ast {" << endl;

	//write the info for the graph nodes
	for (curProc = procs; curProc; curProc= curProc->next)
	{
		int i;
		//write the procedure header node
		outFile << "\t" << curProc->name << " [shape=diamond];" << endl;
	
		//generate the entry for each node in this procedure
		for (curNode = curProc->cfg, i = 0; curNode && i < curProc->size; i++,curNode = curNode->Next())
		{
			outFile << "\t" << curNode->Ident() << " [shape=box";

#ifdef CODEGEN
			if (options.genCode)
				// shade the nodes for code was not generated
				if (curNode->Traversed() != DFS_CODEGEN)
					outFile << ",style=filled";
#endif
			//print the order of the node 
			outFile << ",label=\"" << curNode->Order() + 1;
			
			if (options.revOrder)
			{
				// print the order of the node in the reverse graph
				outFile << "(" << curNode->RevOrder() + 1 << ")";
			}

			if (options.structInfo)
			{
				//print the structured type of the node plus any relevant extra information
				outFile << ":";
				switch (curNode->GetStructType()){
				case Seq:
					outFile << StructString[curNode->GetStructType()];
					break;
				case Cond:
					outFile << CondString[curNode->GetCondType()] << "\\nCF:";
					if (curNode->GetCondFollow())
						outFile << curNode->GetCondFollow()->Order() + 1;
					else
						outFile << "(null)";
					break;
				case Loop:
					outFile << LoopString[curNode->GetLoopType()] << "\\nLT:";
					outFile << curNode->GetLatchNode()->Order() + 1 << "\\nLF:";
					if (curNode->GetLoopFollow())
						outFile << curNode->GetLoopFollow()->Order() + 1;
					else
						outFile << "(null)";
					break;
				case LoopCond:
					outFile << LoopString[curNode->GetLoopType()] << "\\nLT:";
					outFile << curNode->GetLatchNode()->Order() + 1 << "\\nLF:";
					if (curNode->GetLoopFollow())
						outFile << curNode->GetLoopFollow()->Order() + 1;
					else
						outFile << "(null)";
					outFile << "\\nCF:";
					if (curNode->GetCondFollow())
						outFile << curNode->GetCondFollow()->Order() + 1;
					else
						outFile << "(null)";
					break;
				}
			}

			if (options.showHeads)
			{
				// show the loop and case heads for each node
				if (curNode->GetLoopHead())
					outFile << "\\nLH:" << curNode->GetLoopHead()->Order() + 1;
				if (curNode->GetCaseHead())
					outFile << "\\nCH:" << curNode->GetCaseHead()->Order() + 1;
			}
#ifdef NUMBERINGS	
			//print the two timestamp tuples for each node
			outFile << "\\n(" << curNode->loopStamps[0] << "," << curNode->loopStamps[1] << ")";
#endif

			if (options.immPDom)
			{
				// print the immediate dominator info
				if (curNode->GetImmPDom())
					outFile << "\\nImmPDom:" << curNode->GetImmPDom()->Order() + 1;
				else
					outFile << "\\nImmPDom: -";
			}

			//finish off the node
			outFile << "\"];" << endl;
		}

		//build the edge from the procedure block to the first node
		outFile << "\t" << curProc->name << " -> " << curProc->cfg->Ident() << ";" << endl;

		//build the rest of the edges
		for (curNode = curProc->cfg, i = 0; curNode && i < curProc->size; i++,curNode = curNode->Next())
		{
			NodePtrArr const &oEdges = curNode->GetOutEdges();
     		for (int j = 0; j < oEdges.Size(); j++) 
			{
				outFile << "\t" << curNode->Ident() << " -> " << oEdges[j]->Ident();
 				if (curNode->IsJumpToReturn())
					outFile << " [style=dashed];" << endl;
				else if (curNode->GetType() == cBranch && j == THEN)
					outFile << " [style=bold];" << endl;
				else
					outFile << ";" << endl;
			}
		}
	}
	//write the tailer to the file
	outFile << "}";

	outFile.close();
}	
