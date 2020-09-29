/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: Node.h
// Author: Doug Simon
// Purpose: provides a class to represent a node in a control flow graph. As each node also represents
// 	a basic block of assembly instructions, the class has an operation for the initial building 
// 	of these blocks.

#ifndef _NODECLASS_
#define _NODECLASS_

#include "Instruction.h"
#include "DynArr.h"

// We define the indicies for the THEN and ELSE out edges of a two-way conditional
#define THEN 0
#define ELSE 1

// We define a type to represent a pair of time stamps
typedef int* TimeStamps;

// forward declare the Instruction class so that the following typedefs will compile
class CFGNode;

// define a type for an array of node pointers
typedef DynArr<CFGNode*> NodePtrArr;

// an enumerated type for the class of stucture determined for a node
enum structType { 
	Loop,					// Header of a loop only
	Cond,					// Header of a conditional only (if-then-else or switch)
	LoopCond,			// Header of a loop and a conditional
	Seq 					// sequential statement (default)
};

// an type for the class of unstructured conditional jumps
enum unstructType {
	Structured,
	JumpInOutLoop,
	JumpIntoCase
};


// an enumerated type for the type of conditional headers
enum condType {
	IfThen,				// conditional with only a then clause
	IfThenElse,			// conditional with a then and an else clause
	IfElse,				// conditional with only an else clause
	Case					// nway conditional header (case statement)
};

// an enumerated type for the type of loop headers
enum loopType {
	PreTested,				// Header of a while loop
	PostTested,				// Header of a repeat loop
	Endless					// Header of an endless loop
};

// Depth-first traversal constants.
enum travType {
	UNTRAVERSED,			// Initial value
	DFS_TAG,				// Remove redundant nodes pass
	DFS_LNUM,				// DFS loop stamping pass
	DFS_RNUM,				// DFS reverse loop stamping pass
	DFS_CASE, 				// DFS case head tagging traversal
	DFS_PDOM,				// DFS post dominator ordering
	DFS_CODEGEN				// Code generating pass
};

class CFGNode {
public:
#ifdef NUMBERINGS
	friend class Graphs;
#endif
	// constructor sets the identity as well as the member instructions
	CFGNode(int i, Instruction const* first, int num);	

	// destructor cleans up any strings in the node
	~CFGNode();

	// return the unique identifier
	int Ident() const;			

	// returns the type of the node
	bbType GetType() const;			

	// does this node have only one out edge which goes to the return block? i.e. does this
	// block really constitute a mid-function return?
	bool IsJumpToReturn() const;			

	// return the member instructions of this block/node
	InsPtrArr const &Instructions() const;		

	// return the control transfer instruction delimiting this node (if any)
	Instruction const* GetCTI() const;		

	// return the label for the procedure of which this node is the entry
	// node. If it isn't, return 0.	
	char const* GetProcLabel() const;		
														
	// the non-procedure labels at this node. (if any)
	const StrArr &GetNonProcLabels() const;

	// Do a DFS on the graph headed by this node, simply tagging the nodes visited.
	void DfsTag();

	// Do a DFS on the graph headed by this node, giving each node it's time stamp tuple
	// that will be used for loop structuring as well as building the structure that will
	// be used for traversing the nodes in linear time. The inedges are also built during
	// this traversal.
	void SetLoopStamps(int &time, NodePtrArr &order);

#ifndef INTERVALS
	// This sets the reverse loop stamps for each node. The children are traversed in
	// reverse order.
	void SetRevLoopStamps(int &time);
#endif

	// Build the ordering of the nodes in the reverse graph that will be used to
	// determine the immediate post dominators for each node
	void SetRevOrder(NodePtrArr &order);

	// Return the index of this node within the ordering array
	int Order() const;

	// Return the index of this node within the post dominator ordering array
	int RevOrder() const;

#ifndef INTERVALS
	// Is this node within the loop induced by (header,latch)?
	bool InLoop(CFGNode const* header, CFGNode const* latch) const;	
#endif

	// Add an edge from this node to dest. If this is a cBranch type of node and it already
	// has an edge to dest then node edge is added and the node type is changed to fall
	void AddEdgeTo(CFGNode* dest);		

#ifdef INTERVALS
	// Add an edge from src to this node if it doesn't already exist. NB: only interval
	// nodes need this routine as the in edges for normal nodes are built in SetLoopStamps
	void AddEdgeFrom(CFGNode* src);
#endif

	// Return the list of nodes to whom this node has an edge
	NodePtrArr const& GetOutEdges() const;		

	// Return the list of nodes from whom this node has an edge
	NodePtrArr const& GetInEdges() const;		

	// Does this node have an edge to dest?
	bool HasEdgeTo(CFGNode const* dest) const;	

	// Has this node been traversed yet in the current DFS traversal?
	travType Traversed() const;			

	// Set the link to the next node in the list
	void SetNext(CFGNode const* dest);		

	// Return the next node in the list
	CFGNode* Next() const;			

	// Is this node an ancestor of other?
	bool IsAncestorOf(CFGNode const* other) const;	

	// Does this node have a backedge to dest?
	bool HasBackEdgeTo(CFGNode const* dest) const;	

	// Set the pointer to this node's immediate post dominator
	void SetImmPDom(CFGNode const* other);  	

	// Return this node's immediate post dominator
	CFGNode* GetImmPDom() const;		

	// Tag this node as belonging to the loop headed by head
	void SetLoopHead(CFGNode const* head);		

	// Return the head of the most nested loop of which this node is a member
	CFGNode const* GetLoopHead() const;			

	// Set the latch node of a loop header
	void SetLatchNode(CFGNode const* latch);

	// Is this a latch node?
	bool IsLatchNode() const;

	// Pre: this is a loop header
	// Post: Return the latch node
	CFGNode const* GetLatchNode() const;

	// Tag this node and all its children within the case defined by (head,follow)
	// as belonging to the case. If a node visited in this traversal is already with
	// a case then it is left untouched.
	void SetCaseHead(CFGNode const* head, CFGNode const* follow);		

	// Return the head of the most nested case of which this node is a member
	CFGNode const* GetCaseHead() const;			

	// Pre: if this is to be a cond type then the follow (if any) must have 
	// already been determined for this node
	// Set the class of structure determined for this node. 
	void SetStructType(structType s);			

	// Pre: this has already been structured as a two way conditional
	// Sets the restructured type of a two way conditional
	void SetUnstructType(unstructType us);

	// Return the structured type of this node
	structType GetStructType() const;			

	// Return the restructured type of this node
	unstructType GetUnstructType() const;			

	// Pre: the structured class of the node must be Loop or LoopCond
	// Set the loop type of this loop header node
	void SetLoopType(loopType l);			

	// This can be used to override the conditional type derived by the SetStructType routine
	void SetCondType(condType c);

	// Pre: this node must be a loop header and its loop type must be already set.
	// Return the loop type of this node
	loopType GetLoopType() const;			

	// Pre: this node must be a conditional header and its conditional type must be already set.
	// Return the conditional type of this node
	condType GetCondType() const;			

	// Set the node that follows this loop
	void SetLoopFollow(CFGNode const* other);	

	// Return the node that follows this loop
	CFGNode* GetLoopFollow() const;					

	// Set the node that follows this conditional
	void SetCondFollow(CFGNode const* other);	

	// Return the node that follows this conditional
	CFGNode* GetCondFollow() const;					

#ifdef INTERVALS
	// Record the interval to which this node belongs
	void SetInterval(CFGNode* enclosingI);

	// Return the enclosing interval of this node
	CFGNode* InInterval() const;
#endif

	// Emit a goto statement to the given destination as well as making sure that
	// this destination gives itself a label
	void EmitGotoAndLabel(StrArr &HLLCode, int indLevel, CFGNode* dest);

	// Write the code for for this node at the appropriate indentation level
	void WriteCode(StrArr &HLLCode, int indLevel, CFGNode const* latch, NodePtrArr &followSet, NodePtrArr &gotoFollowSet);

#ifdef INTERVALS
protected:
	// Constructor used by the IntNode derived class
	CFGNode(int newId, bbType t) { id = newId; type = t; }
#endif

private:

	int id;								// unique identifier 
	bbType type;						// basic block type
	InsPtrArr instructs;				// member instructions
	NodePtrArr outEdges;				// pointers to the nodes on an out edge from this node
	NodePtrArr inEdges;				// pointers to the nodes on an in edge to this node
	int ord;								// node's position within the ordering structure
	int revOrd;						// node's position within the ordering structure for the reverse graph
	int inEdgesVisited;				// counts the number of in edges visited during a DFS
	int numForwardInEdges;			// number of indges to this node that aren't back edges
	TimeStamps loopStamps, revLoopStamps;	// used for structuring analysis
	travType traversed;				// traversal flag for the numerous DFS's
	CFGNode* next;						// next node in linked list
	bool hllLabel;						// emit a label for this node when generating HL code?
	char* labelStr;					// the high level label for this node (if needed)
	int indentLevel;					// the indentation level of this node in the final code

	// analysis information
	CFGNode* immPDom;					// immediate post dominator
	CFGNode* loopHead;				// head of the most nested enclosing loop
	CFGNode* caseHead;				// head of the most nested enclosing case
	CFGNode* condFollow;				// follow of a conditional header
	CFGNode* loopFollow;				// follow of a loop header
	CFGNode* latchNode;				// latching node of a loop header

	// Structured type of the node
	structType sType;					// the structuring class (Loop, Cond , etc)
	unstructType usType;				// the restructured type of a conditional header
	loopType lType;					// the loop type of a loop header
	condType cType;					// the conditional type of a conditional header
#ifdef INTERVALS
	// Interval membership information
	CFGNode* interval;
#endif

	// return the amount of memory taken up by the member instructions excluding non procedure
	// call CTI's
	int InsSpace() const;

	// Write code for the non-CTI's (excluding procedure calls) in this block at
	// the appropriate indentation level
	void WriteBB(StrArr &HLLCode, int indLevel);

	// Return true if every parent of this node has had its code generated
	bool AllParentsGenerated() const;
};

#endif
