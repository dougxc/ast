/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: Graphs.cpp
//Author: Doug Simon
//Purpose: gives the implementation for the Graphs's operations

#include "Graphs.h"
#include "StringFunctions.h"

#ifdef GETSTATS
#include "Stats.h"
#endif

Graphs::Graphs() :
	nodeList(0), tail(0), procs(0)
{}

void Graphs::append(CFGNode const* node)
{
	if (!nodeList)
		nodeList = tail = (CFGNode*) node;
	else
	{
		tail->SetNext(node);
		tail = tail->Next();
	}
}

void Graphs::BuildNodes(Source const &src)
{
	CFGNode* newNode;		//node currently being built
	Instruction* curIns;	//the current instruction
	Instruction* start;	//the first instruction in a block
	int count;				//number of instructions in a block
	bool isNew = true;	//are we at the beginning of a new block?
	int bId = 1;		//unique identifier for each block

	for (int i = 0; i <src.Size(); i++)
	{
		curIns = &(src[i]);	//define alias

		if (isNew)
		{
			start = curIns;
			count = 1;
		
			isNew = false;
		}
		
		if (curIns->EndBlock())
		{
			newNode = new CFGNode(bId++, start, ++count);
			append(newNode);

			//skip the next instruction as all CTI's have a delayed instruction in our case
			i++;

			//indicate that we are now looking at a new block/node
			isNew = true;
		}
		else if (i < src.Size() - 1 					//can't be the last instruction
			  && (src[i + 1].GetProcLabel() ||			//next instruction is at a label 
					(src[i+1].GetNonProcLabels()).Size() != 0))
		{
			newNode = new CFGNode(bId++, start, count);
			append(newNode);

			//indicate that we are now looking at a new block/node
			isNew = true;
		}
		count++;
	}
#ifdef TESTGRAPHS
	for (newNode = nodeList; newNode != 0; newNode = newNode->Next())
	{
		InsPtrArr const &ins = newNode->Instructions();
		cerr << "Block #" << newNode->Ident() << " is of type " << Type2String(newNode->GetType());
		cerr << " and contans:" << endl;

		for (int i = 0; i < ins.Size(); i++)
			cerr << "\t" << ins[i]->GetString() << endl;
	}
#endif
}

void Graphs::DefineEdges()
{
	NodePtrArr Map;
	Instruction* FirstIns = (nodeList->Instructions())[0];
	int curIdx;
	CFGNode* curNode;

	//Initialise the map size
	curIdx = (int)((tail->Instructions())[(tail->Instructions()).Size() - 1] - FirstIns) + 1;
	Map.Init(curIdx);

	for (curNode = nodeList; curNode; curNode = curNode->Next())
	{
		InsPtrArr const &ins = curNode->Instructions();
		for (int i = 0; i < ins.Size(); i++)
		{
			curIdx = (int)(ins[i] - FirstIns);
			Map[curIdx] = curNode;
		}		
	}

	//build the edges
	for (curNode = nodeList; curNode; curNode = curNode->Next())
	{
		Instruction const* cti = curNode->GetCTI();
      int i;
		switch (curNode->GetType()) {
		case nway:
			for (i = 0; i < (cti->GetJmpDests()).Size(); i++)
			{
				curIdx = (int)((cti->GetJmpDests())[i] - FirstIns);
				curNode->AddEdgeTo(Map[curIdx]);
			}
			break;
		case cBranch:
			// we must add the conditional edges in the order defined by THEN and ELSE
			if (THEN == 1)
			{
				curNode->AddEdgeTo(curNode->Next());
				curIdx = (int)(cti->GetBranchDest() - FirstIns);
				curNode->AddEdgeTo(Map[curIdx]);
			}
			else
			{
				curIdx = (int)(cti->GetBranchDest() - FirstIns);
				curNode->AddEdgeTo(Map[curIdx]);
				curNode->AddEdgeTo(curNode->Next());
			}
			break;
		case uBranch:
			curIdx = (int)(cti->GetBranchDest() - FirstIns);
			curNode->AddEdgeTo(Map[curIdx]);
			break;
		case call:
		case fall:
			curNode->AddEdgeTo(curNode->Next());
			break;
		default:
			break;
		}
	}

	//tag the nodes that are reachable from the head of a procedure.
   for (curNode = nodeList; curNode; curNode = curNode->Next())
		if (curNode->GetProcLabel())
			curNode->DfsTag();

	//remove the blocks that weren't tagged
   for (curNode = nodeList; curNode; curNode = curNode->Next())
	{
#ifdef GETSTATS
		stats.numGraphNodes++;
		stats.numGraphEdges += curNode->GetOutEdges().Size();
#endif
		if (curNode->Next() && curNode->Next()->Traversed() != DFS_TAG)
		{
			CFGNode *tmpNode, *oldNode;
			for(tmpNode = curNode->Next(); tmpNode && tmpNode->Traversed() != DFS_TAG; curNode->SetNext(tmpNode))
			{
#ifdef GETSTATS
				stats.numUnreachIns += tmpNode->Instructions().Size();
#endif
				oldNode = tmpNode;
				tmpNode = tmpNode->Next();
				delete oldNode;
			}
		}
	}
			
#ifdef TESTGRAPHS
   for (curNode = nodeList; curNode; curNode = curNode->Next())
	{
		NodePtrArr const &oEdges = curNode->GetOutEdges();
		NodePtrArr const &iEdges = curNode->GetInEdges();
		cerr << "Node #" << curNode->Ident() << ": ";
		if (oEdges.Size() > 0)
		{
			cerr << " outedges = {";
			for (int i = 0; i < oEdges.Size(); i++)
				cerr << oEdges[i]->Ident() << " ";
			cerr << "}";
		}

		if (iEdges.Size() > 0)
		{
			cerr << " inedges = {";
			for (int i = 0; i < iEdges.Size(); i++)
				cerr << iEdges[i]->Ident() << " ";
			cerr << "}";
		}
		cerr << endl;
	}
#endif
}

void Graphs::DefineCfgs()
{
	ProcHeader* newProc;
	CFGNode* curNode;

   for (curNode = nodeList; curNode; curNode = curNode->Next())
	{
		if (curNode->GetProcLabel())
		{
#ifdef TESTCFGS
			if (procs)
				cerr << "Procedure " << procs->name << " has " << procs->size << " nodes." << endl;
#endif
			newProc = new ProcHeader;
			newProc->name = mystrdup(curNode->GetProcLabel());
			newProc->size = 1;
			newProc->cfg = curNode;
			newProc->exitNode = 0;
			newProc->next = procs;
			procs = newProc;
		}
		else
			newProc->size++;
	
		// is this the return block of this procedure?
		if (curNode->GetType() == ret)
		{
			assert(!newProc->exitNode);
			newProc->exitNode = curNode;
		}
	}
#ifdef TESTCFGS
	if (procs)
		cerr << "Procedure " << procs->name << " has " << procs->size << " nodes." << endl;
#endif
}

#include "GraphsDfs.cc"
#ifdef INTERVALS
#include "GraphsDerSeq.cc"
#endif
#include "Dominators.cc"
#include "Analysis.cc"
#include "GraphsCodeGen.cc"
#include "GraphsPrint.cc"
