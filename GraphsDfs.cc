/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: GraphsDfs.cpp
//Author: Doug Simon
//Purpose: gives the implementation for the Graphs's algorithm dependant,
//	depth first traversal labelling  operation. Also the comparison operations that
//	are performed in the context of this ordering

void Graphs::SetTimeStamps()
{
	//do the time stamping used for loop structuring 
	for (ProcHeader* curProc = procs; curProc; curProc = curProc->next)
	{
		int time = 1;
		NodePtrArr &order = curProc->Ordering;

		order.Init(curProc->size);

		// set the parenthesis for the nodes as well as setting
		// the post-order ordering between the nodes
		curProc->cfg->SetLoopStamps(time,order);

#ifndef INTERVALS
		// set the reverse parenthesis for the nodes
		time = 1;
		curProc->cfg->SetRevLoopStamps(time);
#endif
	}

	// do the ordering of nodes within the reverse graph 
	for (ProcHeader* curProc = procs; curProc; curProc = curProc->next)
	{
		NodePtrArr &order = curProc->revOrdering;

		order.Init(curProc->size);

		assert(curProc->exitNode);
		curProc->exitNode->SetRevOrder(order);
	}	
}
