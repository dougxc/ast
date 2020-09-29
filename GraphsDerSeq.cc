/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: GraphsClassDerSeq.cpp
// Author: Doug Simon
// Description: implements the BuildDerivedSequences operation of the Graphs class.

#include <string.h>
#include "Interval.h"
#include "MemAdvise.h"
#include "Stats.h"

bool SubSetOf(NodePtrArr const &iEdges, IntNode* newInt)
{
	for (int i = 0; i < iEdges.Size(); i++)
		if (iEdges[i]->InInterval() != newInt)
			return false;
	return true;
}

void Graphs::BuildIntervals(DerivedGraph* derGraph)
{
	assert(derGraph && derGraph->cfg);

	NodePtrArr &intSeq = derGraph->intervals;	// The sequence of intervals in this graph
	NodePtrArr headerSeq;	// The sequence of interval header nodes
	NodePtrArr beenInH;		// The set of nodes that have been in the above sequence at some stage

	// Initialise the header sequence to contain the head of the graph
	headerSeq.Add(derGraph->cfg);

	// Note that this node has now been in the header sequence
	beenInH.Add(derGraph->cfg);

	// Keep processing the header sequence until it is empty
	while (headerSeq.Size() != 0)
	{
		// Remove the head of the headers sequence and set it to be the head of a new interval
		CFGNode* header = headerSeq[0];
		IntNode* newInt = new IntNode(header);
		headerSeq.RemoveFirst();

		// Process each succesive node in the interval until no more nodes can be added to the interval.
		for (int i = 0; i < newInt->Nodes().Size(); i++)
		{
			CFGNode* curNode = newInt->Nodes()[i];

			// Process each child of the current node
			for (int j = 0; j < curNode->GetOutEdges().Size(); j++)
			{
				CFGNode* succ = curNode->GetOutEdges()[j];

				// Only further consider the current child if it isn't already in the interval
				if (!newInt->Nodes().IsIn(succ))
				{
					// If the current child has all its parents
					// inside the interval, then add it to the interval. Remove it from the header
					// sequence if it is on it.
					if (SubSetOf(succ->GetInEdges(),newInt))
					{
						newInt->AddNode(succ);
						if (headerSeq.IsIn(succ))
							headerSeq.Remove(succ);
					}
		
					// Otherwise, add it to the header sequence if it hasn't already been in it.
					else if (!beenInH.IsIn(succ))
					{
						headerSeq.Add(succ);
						beenInH.Add(succ);
					}	
				}
			}
		}

		// Add the new interval to the sequence of intervals
		intSeq.Add(/*static_cast<CFGNode*>*/(newInt));
	}
#ifdef TESTINTS
	cerr << "Just built the following intervals:" << endl;
	cerr << "Interval \tNodes in interval" << endl;

	for (int i = 0; i < intSeq.Size(); i++)
	{
		IntNode* curInt = static_cast<IntNode*>(intSeq[i]);
		cerr << "\t" << curInt->Ident() << "\t\t";
		NodePtrArr const &nodes = curInt->Nodes();
		for (int j = 0; j < nodes.Size(); j++)
			cerr << (nodes[j]->GetType() != intNode ? nodes[j]->Order() : nodes[j]->Ident()) << ",";
		cerr << endl;
	}

	
#endif
}

void Graphs::BuildNextOrderGraph(DerivedGraph* curGraph, DerivedGraph* nextGraph)
// Pre: the number of intervals in the current graph is greater than the number of nodes
//		  in the current graph. 
// Post: the next order graph has been built
{
	assert(curGraph->numOfNodes > curGraph->intervals.Size());

	// Set the first interval in the list of intervals for the current graph to be the head
	// of the next order graph
	nextGraph->cfg = curGraph->intervals[0];

	// Process the intervals of the current graph (which are the nodes in the next order graph)
	// to define the edges between them
	for (int i = 0; i < curGraph->intervals.Size(); i++)
	{
		IntNode* curInt = static_cast<IntNode*>(curGraph->intervals[i]);

		// Process each node in the current interval to see if if has an outedge to another interval
		for (int j = 0; j < curInt->Nodes().Size(); j++)
		{
			CFGNode* curNode = curInt->Nodes()[j];

			// Examine each out edge of the current node within the interval
			for (int k = 0; k < curNode->GetOutEdges().Size(); k++)
			{
				CFGNode* child = curNode->GetOutEdges()[k];
	
				// If the outedge under examination leads outside the current interval, then add it as
				// an edge to this interval. Also add the corresponding in edges to the child
				if (child->InInterval() != curInt)
				{
					curInt->AddEdgeTo(child->InInterval());
					child->InInterval()->AddEdgeFrom(curInt);
				}
			}
		}
	}
}

void Graphs::BuildDerivedSequence(ProcHeader* proc)
{
	// Initialise the first graph in the sequence to be the CFG for the procedure
	DerivedGraph* curGraph = new DerivedGraph;
	curGraph->cfg = proc->cfg;
	curGraph->numOfNodes = proc->size;
	proc->derivedGraphs.Add(curGraph);

	// Continually process the current graph until it is the trivial graph (i.e. it has only one node)
	while (curGraph->numOfNodes != 1)
	{
		// Find the intervals in the current graph
		BuildIntervals(curGraph);

		// If the number of intervals found is equal to the number of nodes in the graph, then this
		// graph is irreducible and the generation of the derived sequence must be terminated here
		// to prevent infinite recursion.
		if (curGraph->numOfNodes == curGraph->intervals.Size())
			break;

#ifdef GETSTATS
		stats.numDerGraphs++;
		stats.numIntervals += curGraph->intervals.Size();
#endif 

		// Build the next order graph 
		DerivedGraph* nextGraph = new DerivedGraph;
		nextGraph->numOfNodes = curGraph->intervals.Size();
		BuildNextOrderGraph(curGraph,nextGraph);

		// Add the next order graph to the sequence of derived graphs
		proc->derivedGraphs.Add(nextGraph);

		// Make the next order graph be the next one to be reduced
		curGraph = nextGraph;
	}

	// If the generation of the derived sequence resulted in the trivial graph, then this trivial
	// graph still needs its (trivial) singleton set of intervals built
	assert(curGraph == proc->derivedGraphs[proc->derivedGraphs.Size() - 1]);
	if (curGraph->numOfNodes == 1)
		BuildIntervals(curGraph);
}

void Graphs::DisplayIntervals(NodePtrArr const &intSeq)
// Pre: the interval sequence to be printed has been built
// Post: for each interval, its numeric identifier and member nodes have been displayed
{
	for (int i = 0; i < intSeq.Size(); i++)
	{
		IntNode* curInt = static_cast<IntNode*>(intSeq[i]);

		cout << "   Interval #" << curInt->Ident() << ":" << endl;

		for (int j = 0; j < curInt->Nodes().Size(); j++)
		{
			CFGNode* curNode = curInt->Nodes()[j];
			cout << "      " << ((curNode->GetType() != intNode) ? "BB node #" : "IntNode #");
			cout << ((curNode->GetType() != intNode) ? curNode->Order() : curNode->Ident()) << endl;
		}
	}
}	

void Graphs::DisplayDerivedSequence(ProcHeader* proc)
// Pre: the derived sequence to be displayed has been built
// Post: the intervals and nodes within each derived graph have been displayed
{
	cout << "Derived sequence intervals for procedure " << proc->name << endl;

	for (int i = 0; i < proc->derivedGraphs.Size(); i++)
	{
		DerivedGraph* curGraph = proc->derivedGraphs[i];
		cout << "\nDerived graph #" << i << ":" << endl;
		DisplayIntervals(curGraph->intervals);
	}

	// Indicate whether or not the graph was reducible
	if (proc->derivedGraphs[proc->derivedGraphs.Size() - 1]->numOfNodes == 1)
		cout << "The graph is reducible." << endl;
	else
		cout << "The graph is not reducible." << endl;

}

void Graphs::BuildDerivedSequences()
{
#ifdef GETSTATS
	double t[3] = {0,0,0};	// for dtime
	dtime(t);
	stats.derSeqMemCost  = MemStats[2];
	stats.derSeqMemAlloc = MemStats[0];
#endif

	for (ProcHeader* curProc = procs; curProc; curProc = curProc->next)
		BuildDerivedSequence(curProc);
#ifdef GETSTATS
	dtime(t);
	stats.bldDerSeqTime = t[1];
	stats.derSeqMemCost  = MemStats[2] - stats.derSeqMemCost;
	stats.derSeqMemAlloc = MemStats[0] - stats.derSeqMemAlloc;
#endif
}

void Graphs::DisplayDerivedSequences()
{
	for (ProcHeader* curProc = procs; curProc; curProc = curProc->next)
		DisplayDerivedSequence(curProc);
}
