/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: Interval.cpp
// Author: Doug Simon
// Description: implements the operations of the IntNode class.

#include "Node.h"
#include "Interval.h"

// Define a global variable to be used when assigning unique id's to new interval nodes
static int NewIntId = 0;

IntNode::IntNode(CFGNode* node) : CFGNode(NewIntId++,intNode)
{
	AddNode(node);	
}

bool IntNode::IsIn(CFGNode* node)
{
	return nodes.IsIn(node);
}

void IntNode::AddNode(CFGNode* node)
{
	nodes.Add(node);
	node->SetInterval(this);
}

NodePtrArr const &IntNode::Nodes() const
{
	return nodes;
}

void IntNode::FindNodesInInt(bool* &cfgNodes, int level)
{
	if (level == 0)
		for (int i = 0; i < nodes.Size(); i++)
			cfgNodes[nodes[i]->Order()] = true;
	else
		for (int i = 0; i < nodes.Size(); i++)
			static_cast<IntNode*>(nodes[i])->FindNodesInInt(cfgNodes, level - 1);
}
