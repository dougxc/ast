/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: Node.cpp
//Author: Doug Simon
//Purpose: provides the implementation of the CFGNode class's operations

#include "Node.h"
#include <assert.h>
#include <string.h>

CFGNode::CFGNode(int i, Instruction const* first, int num) :
	id(i), instructs(num), ord(-1), revOrd(-1), inEdgesVisited(0), 
	numForwardInEdges(-1), traversed(UNTRAVERSED), next(NULL), hllLabel(false), 
	labelStr(0), indentLevel(0), immPDom(NULL), loopHead(NULL), caseHead(NULL),
	condFollow(NULL), loopFollow(NULL), latchNode(NULL), sType(Seq), 
	usType(Structured) 
#ifdef INTERVALS
	, interval(NULL)
#endif
{
	int delimit;	//index of the delimiting instruction

	//initialize the two timestamp tuples
	loopStamps = new int[2];
	revLoopStamps = new int[2];
	for (int i = 0; i < 2; i++)
		loopStamps[i] = revLoopStamps[i] = -1;

	//build the array of pointers
	for (int i = 0; i < num; i++)
		instructs.Add((Instruction*)first++);

	//determine the type of the block
	delimit = (num > 1) ? num - 2 : 0;
	type = iType2bbType(instructs[delimit]->GetType());

	//initialise the size of the out edges array according to this type
	switch (type) {
	case cBranch:
		outEdges.Init(2);
		break;
	case uBranch:
	case call:
	case fall:
		outEdges.Init(1);
		break;
	case nway:
		outEdges.Init((instructs[delimit]->JmpDestLabels()).Size());
		break;
	case ret:
	default:
		break;
	}
}

//bool CFGNode::operator==(CFGNode const& other) const { return &other == this; }
//bool CFGNode::operator!=(CFGNode const& other) const { return &other != this; }
CFGNode::~CFGNode()
{
	if (labelStr) delete labelStr;
}

int CFGNode::Ident() const { return id; }
bbType CFGNode::GetType() const { return type; }
bool CFGNode::IsJumpToReturn() const 
{ 
	return (type == uBranch && outEdges[0]->type == ret &&
			  outEdges[0]->instructs.Size() == 2); 
}

InsPtrArr const &CFGNode::Instructions() const { return instructs; }
int CFGNode::InsSpace() const
{
	int space = 0;
	for (int i = 0; i < instructs.Size(); i++)
		space += strlen(instructs[i]->GetString()) + 1;

	// subtract the space taken up a non-procedure call CTI (if any)
	if (type == cBranch || type == uBranch || type == nway || type == ret)
		space -= strlen(instructs[instructs.Size() - 2]->GetString()) + 1;

	return space;
}

Instruction const* CFGNode::GetCTI() const 
{
	//return NULL if this is a fall through node
	if (type == fall)
		return NULL;

	//else return a pointer to the 2nd last instruction in this block which will
	//invariably be a CTI
	else
		return instructs[instructs.Size() - 2];
}

char const* CFGNode::GetProcLabel() const { return instructs[0]->GetProcLabel(); }
const StrArr &CFGNode::GetNonProcLabels() const {return instructs[0]->GetNonProcLabels(); }

void CFGNode::DfsTag()
{
	traversed = DFS_TAG;
	for (int i = 0; i < outEdges.Size(); i++)
		if (outEdges[i]->traversed != DFS_TAG)
			outEdges[i]->DfsTag();
}

void CFGNode::SetLoopStamps(int &time, NodePtrArr &order)
{
	//timestamp the current node with the current time and set its traversed flag
	traversed = DFS_LNUM;
	loopStamps[0] = time;

	//recurse on unvisited children and set inedges for all children
	for (int i = 0; i < outEdges.Size(); i++)
	{
		// set the in edge from this child to its parent (the current node)
		outEdges[i]->inEdges.Add(this);
		
		// recurse on this child if it hasn't already been visited
		if (outEdges[i]->traversed != DFS_LNUM)
			outEdges[i]->SetLoopStamps(++time,order);
	}

	//set the the second loopStamp value
	loopStamps[1] = ++time;

	//add this node to the ordering structure as well as recording its position within the ordering
	ord = order.Size();
	order.Add(this);
}

#ifndef INTERVALS
void CFGNode::SetRevLoopStamps(int &time)
{
	//timestamp the current node with the current time and set its traversed flag
	traversed = DFS_RNUM;
	revLoopStamps[0] = time;

	//recurse on the unvisited children in reverse order
	for (int i = outEdges.Size() - 1; i >= 0; i--)
	{
		// recurse on this child if it hasn't already been visited
		if (outEdges[i]->traversed != DFS_RNUM)
			outEdges[i]->SetRevLoopStamps(++time);
	}

	//set the the second loopStamp value
	revLoopStamps[1] = ++time;
}
#endif

void CFGNode::SetRevOrder(NodePtrArr &order)
{
	// Set this node as having been traversed during the post domimator 
	// DFS ordering traversal
	traversed = DFS_PDOM;
	
	// recurse on unvisited children 
	for (int i = 0; i < inEdges.Size(); i++)
		if (inEdges[i]->traversed != DFS_PDOM)
			inEdges[i]->SetRevOrder(order);

	// add this node to the ordering structure and record the post dom. order
	// of this node as its index within this ordering structure
	revOrd = order.Size();
	order.Add(this);
}

int CFGNode::Order() const 
{ 
	assert(ord != -1);
	return ord;
}

int CFGNode::RevOrder() const
{
	assert(revOrd != -1);
	return revOrd;
}

#ifndef INTERVALS
bool CFGNode::InLoop(CFGNode const* header, CFGNode const* latch) const
{
	assert(header->latchNode == latch);
	assert(header == latch || ((header->loopStamps[0] > latch->loopStamps[0] && latch->loopStamps[1] > header->loopStamps[1])||
		(header->loopStamps[0] < latch->loopStamps[0] && latch->loopStamps[1] < header->loopStamps[1])));

	return(
		//this node is in the loop if it is the latch node OR...
		(this == latch) ||

		//...this node is within the header and the latch is within this 
		// when using the forward loop stamps OR...
		(header->loopStamps[0] < loopStamps[0] && loopStamps[1] < header->loopStamps[1] &&
		 loopStamps[0] < latch->loopStamps[0] && latch->loopStamps[1] < loopStamps[1]) ||

		//...this node is within the header and the latch is within this 
		// when using the reverse loop stamps 
		(header->revLoopStamps[0] < revLoopStamps[0] && revLoopStamps[1] < header->revLoopStamps[1] &&
		 revLoopStamps[0] < latch->revLoopStamps[0] && latch->revLoopStamps[1] < revLoopStamps[1]));
}
#endif

void CFGNode::AddEdgeTo(CFGNode* dest) 
{ 
	if (type != cBranch || !HasEdgeTo(dest))
		outEdges.Add(static_cast<CFGNode*>(dest)); 
	else
		//reset the type to fall if no edge was added (i.e. this edge already existed)
		type = fall;
}

#ifdef INTERVALS
void CFGNode::AddEdgeFrom(CFGNode* src) 
{ 
	if (!inEdges.IsIn(src))
		inEdges.Add(static_cast<CFGNode*>(src)); 
}
#endif

NodePtrArr const &CFGNode::GetOutEdges() const { return outEdges; }
NodePtrArr const &CFGNode::GetInEdges() const { return inEdges; }
bool CFGNode::HasEdgeTo(CFGNode const *dest) const
{
	for (int i = 0; i < outEdges.Size(); i++)
		if (outEdges[i] == dest)
			return true;
	return false;
}

travType CFGNode::Traversed() const { return traversed; }

void CFGNode::SetNext(CFGNode const* dest) { next = (CFGNode*)dest; }
CFGNode* CFGNode::Next() const { return next; }

bool CFGNode::IsAncestorOf(CFGNode const* other) const
{           
    return ((loopStamps[0] < other->loopStamps[0] && 
				 loopStamps[1] > other->loopStamps[1]) ||
    			(revLoopStamps[0] < other->revLoopStamps[0] && 
				 revLoopStamps[1] > other->revLoopStamps[1]));
}   
        
bool CFGNode::HasBackEdgeTo(CFGNode const* dest) const
{
	assert(HasEdgeTo(dest) || dest == this);
	return (dest == this || dest->IsAncestorOf(this));
}

void CFGNode::SetImmPDom(CFGNode const* other) { immPDom = (CFGNode*)other; }
CFGNode* CFGNode::GetImmPDom() const { return immPDom; }

void CFGNode::SetLoopHead(CFGNode const* head) { loopHead = (CFGNode*) head; }
CFGNode const* CFGNode::GetLoopHead() const { return loopHead; }

void CFGNode::SetLatchNode(CFGNode const* latch)
{
	latchNode = (CFGNode*)latch;
}

bool CFGNode::IsLatchNode() const
{
	return(loopHead && loopHead->latchNode == this);
}

CFGNode const* CFGNode::GetLatchNode() const 
{ 
	return latchNode; 
}

void CFGNode::SetCaseHead(CFGNode const* head, CFGNode const* follow) 
{
	assert(!caseHead);

	traversed = DFS_CASE;

	// don't tag this node if it is the case header under investigation 
	if (this != head)
		//caseHead = static_cast<CFGNode*>(head);
		caseHead = (CFGNode*)(head);

	// if this is a nested case header, then it's member nodes will already have been
	// tagged so skip straight to its follow
	if (type == nway && this != head)
	{
		if (condFollow->traversed != DFS_CASE && condFollow != follow)
			condFollow->SetCaseHead(head, follow);
	}
	else
		// traverse each child of this node that:
		//   i) isn't on a back-edge,
		//  ii) hasn't already been traversed in a case tagging traversal and,
		// iii) isn't the follow node.
		for (int i = 0; i < outEdges.Size(); i++)
			if (!HasBackEdgeTo(outEdges[i]) && outEdges[i]->traversed != DFS_CASE && outEdges[i] != follow)
				outEdges[i]->SetCaseHead(head, follow);
}
		
CFGNode const* CFGNode::GetCaseHead() const { return caseHead; }

void CFGNode::SetStructType(structType s)
{
	// if this is a conditional header, determine exactly which type of 
	// conditional header it is (i.e. switch, if-then, if-then-else etc.)
	if (s == Cond)
	{
		if (type == nway) 
			cType = Case;
		else if (outEdges[ELSE] == condFollow)
			cType = IfThen;
		else if (outEdges[THEN] == condFollow)
			cType = IfElse;
		else
			cType = IfThenElse;
	}

	sType = s;
}

void CFGNode::SetUnstructType(unstructType us)
{
	assert((sType == Cond || sType == LoopCond) && cType != Case);
	usType = us;
}

structType CFGNode::GetStructType() const { return sType; }
unstructType CFGNode::GetUnstructType() const
{
	assert((sType == Cond || sType == LoopCond) && cType != Case);
	return usType;
}

void CFGNode::SetLoopType(loopType l)
{
	assert (sType == Loop || sType == LoopCond);
	lType = l;

	//set the structured class (back to) just Loop if the loop type is PreTested OR
	//it's PostTested and is a single block loop
	if (lType == PreTested || (lType == PostTested && this == latchNode))
		sType = Loop;
}

loopType CFGNode::GetLoopType() const
{
	assert (sType == Loop || sType == LoopCond);
	return lType;
}

void CFGNode::SetCondType(condType c)
{
	assert (sType == Cond || sType == LoopCond);
	cType = c;
}

condType CFGNode::GetCondType() const
{
	assert (sType == Cond || sType == LoopCond);
	return cType;
}

void CFGNode::SetLoopFollow(CFGNode const* other) { loopFollow = (CFGNode*)other; }
CFGNode* CFGNode::GetLoopFollow() const { return loopFollow; }

void CFGNode::SetCondFollow(CFGNode const* other) { condFollow = (CFGNode*)other; }
CFGNode* CFGNode::GetCondFollow() const { return condFollow; }

#ifdef INTERVALS
void CFGNode::SetInterval(CFGNode* enclosingI) { interval = enclosingI; }
CFGNode* CFGNode::InInterval() const { return interval; }
#endif

// Include the operations that generate the HLL code
#include "NodeCodeGen.cc"
