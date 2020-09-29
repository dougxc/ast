/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: IntClass.h
// Author: Doug Simon
// Description: provide a class for the intervals built during the structuring of a control
//		flow graph.

#ifndef _INTCLASS_
#define _INTCLASS_

#include "Node.h"

class IntNode : public CFGNode {
public:
	// Constructor that requires the head node of the new interval
	IntNode(CFGNode* node);
	
	// Test for membership of a node
	bool IsIn(CFGNode* node);

	// Add a new node to the set of nodes in this interval. Also set the interval membership
	// for the node being added
	void AddNode(CFGNode* node);

	// Return the set of nodes in this interval
	NodePtrArr const &Nodes() const;

	// Recursive routine that updates the map from low-level nodes to their membership within
	// this interval
	void FindNodesInInt(bool* &cfgNodes, int level);

private:
	NodePtrArr nodes;		// nodes of the interval
};

#endif
