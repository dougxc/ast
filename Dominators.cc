/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: Dominators.cpp
// Author: Doug Simon
// Purpose: Provides the operations to define the immediate post dominators for each node.

#include <assert.h>
#include <iostream.h>

//********************************************************************************
// Immediate Post-Dominator routines
//********************************************************************************
CFGNode* Graphs::CommonPDom(CFGNode* curImmPDom, CFGNode* succImmPDom)
// Finds the common post dominator of the current immediate post dominator
// and its successor's immediate post dominator
{
	if (!curImmPDom)
		return succImmPDom;
	if (!succImmPDom)
		return curImmPDom;

	while (curImmPDom && succImmPDom && (curImmPDom != succImmPDom))
		if (curImmPDom->RevOrder() > succImmPDom->RevOrder())
			succImmPDom = succImmPDom->GetImmPDom();
		else
			curImmPDom = curImmPDom->GetImmPDom();

	return (curImmPDom);
}

void Graphs::FindImmedPDom (ProcHeader* curProc)
/* Finds the immediate post dominator of each node in the graph PROC->cfg.
 * Adapted version of the dominators algorithm by Hecht and Ullman; finds
 * immediate post dominators only.
 * Note: graph should be reducible */
{
	CFGNode *curNode, *succNode;	// the current Node and its successor
	NodePtrArr &revOrder = curProc->revOrdering;

	// traverse the nodes in order (i.e from the bottom up)
	for (int i = revOrder.Size() - 1; i >= 0; i--)
	{
		curNode = revOrder[i];
		NodePtrArr const &oEdges = curNode->GetOutEdges();
		for (int j = 0; j < oEdges.Size(); j++) 
		{
			succNode = oEdges[j];
			if (succNode->RevOrder() > curNode->RevOrder())
				curNode->SetImmPDom(CommonPDom(curNode->GetImmPDom(), succNode));
		}
	}

	// make a second pass but consider the original CFG ordering this time
	NodePtrArr &order = curProc->Ordering;
	for (int i = 0; i < order.Size(); i++)
	{
		curNode = order[i];
		NodePtrArr const &oEdges = curNode->GetOutEdges();
		if (oEdges.Size() > 1)
			for (int j = 0; j < oEdges.Size(); j++) 
			{
				succNode = oEdges[j];
					curNode->SetImmPDom(CommonPDom(curNode->GetImmPDom(), succNode));
			}
	}

	// one final pass to fix up nodes involved in a loop
	for (int i = 0; i < order.Size(); i++)
	{
		curNode = order[i];
		NodePtrArr const &oEdges = curNode->GetOutEdges();
		if (oEdges.Size() > 1)
			for (int j = 0; j < oEdges.Size(); j++) 
			{
				succNode = oEdges[j];	
				if (curNode->HasBackEdgeTo(succNode) && curNode->GetOutEdges().Size() > 1 &&
					 succNode->GetImmPDom()->Order() < curNode->GetImmPDom()->Order())
					curNode->SetImmPDom(CommonPDom(succNode->GetImmPDom(),curNode->GetImmPDom()));
				else
					curNode->SetImmPDom(CommonPDom(curNode->GetImmPDom(),succNode));
			}
	}

#ifdef TESTPOSTDOM
	cerr << "\nImmediate post dominator info for procedure " << curProc->name << endl;
	cerr << "Node\t| ImmPDom" << endl;
	for (int i = 0; i < order.Size(); i++)
	{
		curNode = order[i];
		// every node must either have a post immediate dominator or be the exit node of a procedure
		assert(curNode->GetImmPDom() || curNode->GetOutEdges().Size() == 0);

		cerr << curNode->Order();
		if (curNode->GetImmPDom())
			cerr << "\t|  " << curNode->GetImmPDom()->Order();
		else
			cerr << "\t|  -";
		cerr << endl;
	}
#endif
}
