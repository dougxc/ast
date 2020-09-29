/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: Graphs.h
// Author: Doug Simon
// Purpose: Provides a class to store all the control flow graphs of an assembly program.

#ifndef _GRAPHSCLASS_
#define _GRAPHSCLASS_

#include "Node.h"
#include "Source.h"
#include "Instruction.h"
#include "TypeDefs.h"

#ifdef INTERVALS
// define a type to store the information about a derived graph
struct DerivedGraph {
	CFGNode* cfg;				// head of derived graph
	int numOfNodes;			// number of nodes in this graph
	NodePtrArr intervals;	// intervals of derived graph
};

// define a type to store a sequence of derived graphs
typedef DynArr<DerivedGraph*> DGPtrArr;
#endif

class Graphs {
public:

	// default constructor
	Graphs();						

	// build the set of nodes from the source instructions
	void BuildNodes(Source const &src);		

	// define the edges between these nodes as well as removing any redundant nodes
	void DefineEdges();					

	// build the headers for each cfg within the program
	void DefineCfgs();					

	// Give each node its DFS generated orderings.
	void SetTimeStamps();				

#ifdef INTERVALS
	// Build the sequence of derived graphs for each procedure
	void BuildDerivedSequences();

	// Display the sequence of derived graphs for each procedure
	void DisplayDerivedSequences();
#endif

	// apply the structuring algorithm to each CFG
	void Structure();				

	// generate the structured HLL code and only perform the goto removal post
	// processor if required by the command line arguments
	void CodeGen(char* fname);

	// generate graphviz output and store it in fname
	void GenerateGraphvizFile(char* fname);	

private:
	CFGNode* nodeList;			// head of the linked list of nodes
	CFGNode* tail;					// tail of the linked list of nodes (next insertion point)

	struct ProcHeader {
		CFGNode* cfg;					// The node at the head of the graph
		CFGNode* exitNode;			// the node at the bottom of the graph
		int size;
		char* name;
		NodePtrArr Ordering;		// an array of pointers to the nodes
											// within this procedure such that the nodes lower in
											// graph are earlier in the array
		NodePtrArr revOrdering;	// an array of pointers to the nodes
											// within this procedure such that the nodes lower in
											// reverse graph are earlier in the array
#ifdef INTERVALS
		DGPtrArr derivedGraphs;	// the derived graphs for this procedure
#endif
		ProcHeader* next;
	}* procs;

	
	void append(CFGNode const* node);	// append a node onto the list of nodes
	void DfsTag(CFGNode* curNode);		// do a dfs on the list of nodes
	void DfsVisit(CFGNode* curNode, int &time, NodePtrArr &revOrder);

	void FindImmedPDom (ProcHeader* curProc);
	CFGNode* CommonPDom (CFGNode* curImmPDom, CFGNode* predImmPDom);

	void StructLoops(ProcHeader* curProc);
	void StructConds(ProcHeader* curProc);
	void CheckConds(ProcHeader* curProc);

#ifdef INTERVALS
	// Build the intervals for a given derived graph
	// Pre: space must have been allocated for the derived graph and its cfg header must have been set
	// Post: the derived graphs now contains its list of intervals
	void BuildIntervals(DerivedGraph* derGraph);
	
	// Build the derived sequence of graphs within a CFG
	void BuildDerivedSequence(ProcHeader* proc);

	// Pre: the number of nodes in the current graph must be greater than the number of intervals 
	//		  determined for the current graph
	// Build the next order graph from the current derived graph
	void BuildNextOrderGraph(DerivedGraph* curGraph, DerivedGraph* nextGraph);

	void DisplayIntervals(NodePtrArr const &intSeq);
	
	void DisplayDerivedSequence(ProcHeader* proc);
#endif
};
	
#endif
