/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// Author: Doug Simon
// Purpose: Provides the implementation for the operations of a Node object that
// 	involve the actual control flow analysis

#include <assert.h>
#include "MemAdvise.h"
#include "Stats.h"

// an inline function to test whether a given node has a back edge
bool HasABackEdge(CFGNode* curNode) 
{
	for (int i = 0; i < curNode->GetOutEdges().Size(); i++)
		if (curNode->HasBackEdgeTo(curNode->GetOutEdges()[i]))
			return true;
	return false;
}

//********************************************************************************
// Conditional structuring routines
//********************************************************************************
void Graphs::StructConds(ProcHeader* curProc)
// Structures all conditional headers (i.e. nodes with more than one outedge)
{
	NodePtrArr const &order = curProc->Ordering;

	// Process the nodes in order
	for (int i = 0; i < order.Size(); i++)
	{
		CFGNode* curNode = order[i];

		// does the current node have more than one out edge?
		if (curNode->GetOutEdges().Size() > 1)
		{
			// if the current conditional header is a two way node and has a back edge, 
			// then it won't have a follow
			if (HasABackEdge(curNode) && curNode->GetType() == cBranch)
			{
				curNode->SetStructType(Cond);
				continue;
			}
		
			// set the follow of a node to be its immediate post dominator
			curNode->SetCondFollow(curNode->GetImmPDom());

			// set the structured type of this node
			curNode->SetStructType(Cond);

			// if this is an nway header, then we have to tag each of the nodes
			// within the body of the nway subgraph
			if (curNode->GetCondType() == Case)
				curNode->SetCaseHead(curNode,curNode->GetCondFollow());
		}
	}

}

//********************************************************************************
// Loop structuring routines
//********************************************************************************
#ifndef INTERVALS
void DetermineLoopType(CFGNode* header, bool* &loopNodes)
#else
void DetermineLoopType(CFGNode* header)
#endif
// Pre: The loop induced by (head,latch) has already had all its member nodes tagged
// Post: The type of loop has been deduced
{
	assert(header->GetLatchNode());

	// if the latch node is a two way node then this must be a post tested loop
	if (header->GetLatchNode()->GetType() == cBranch)
	{
		header->SetLoopType(PostTested);
		
		// if the head of the loop is a two way node and the loop spans more than one block
		// then it must also be a conditional header
		if (header->GetType() == cBranch && header != header->GetLatchNode())
			header->SetStructType(LoopCond);
	}

	// otherwise it is either a pretested or endless loop
	else if (header->GetType() == cBranch)
	{
	#ifndef INTERVALS
		// if the header is a two way node then it must have a conditional follow (since it
		// can't have any backedges leading from it). If this follow is within the loop then
		// this must be an endless loop
		assert(header->GetCondFollow());
		if (loopNodes[header->GetCondFollow()->Order()])
		{
			header->SetLoopType(Endless);

			// retain the fact that this is also a conditional header
			header->SetStructType(LoopCond);
		}
		else
		header->SetLoopType(PreTested);
	#else
		// if the header is a two way conditional header, then it will be a pretested loop
		// if one of its children is its conditional follow
		if (header->GetOutEdges()[0] != header->GetCondFollow() && header->GetOutEdges()[1] != header->GetCondFollow())
		{
			// neither children are the conditional follow
			header->SetLoopType(Endless);

			// retain the fact that this is also a conditional header
			header->SetStructType(LoopCond);
		}
		else
			// one child is the conditional follow
			header->SetLoopType(PreTested);
	#endif
	}

	// both the header and latch node are one way nodes so this must be an endless loop
	else
		header->SetLoopType(Endless);
}

void FindLoopFollow(CFGNode* header, NodePtrArr const &order, bool* &loopNodes)
// Pre: The loop headed by header has been induced and all it's member nodes have been tagged
// Post: The follow of the loop has been determined.
{
	assert(header->GetStructType() == Loop || header->GetStructType() == LoopCond);
	loopType lType = header->GetLoopType();
	CFGNode const* latch = header->GetLatchNode();

	if (lType == PreTested)
	{
	#ifndef INTERVALS
		// if the 'while' loop's true child is within the loop, then its false child is
		// the loop follow
		if (loopNodes[header->GetOutEdges()[0]->Order()])
			header->SetLoopFollow(header->GetOutEdges()[1]);

		// otherwise the true child is the loop follow
		else
			header->SetLoopFollow(header->GetOutEdges()[0]);
	#else
		// the child that is the loop header's conditional follow will be the loop follow
		if (header->GetOutEdges()[0] == header->GetCondFollow())
			header->SetLoopFollow(header->GetOutEdges()[0]);
		else
			header->SetLoopFollow(header->GetOutEdges()[1]);
	#endif
	}
	else if (lType == PostTested)
	{
		// the follow of a post tested ('repeat') loop is the node on the end of the
		// non-back edge from the latch node
		if (latch->GetOutEdges()[0] == header)
			header->SetLoopFollow(latch->GetOutEdges()[1]);
		else
			header->SetLoopFollow(latch->GetOutEdges()[0]);
	}
	else // endless loop
	{
		CFGNode* follow = NULL;
	
		// traverse the ordering array between the header and latch nodes.
		CFGNode const* latch = header->GetLatchNode();
		for (int i = header->Order() - 1; i > latch->Order(); i--)
		#ifndef INTERVALS
		{
			CFGNode* &desc = order[i];
			// the follow for an endless loop will have the following properties:
			//   i) it will have a parent that is a conditional header inside the loop whose follow
			//      is outside the loop
			//  ii) it will be outside the loop according to its loop stamp pair
			// iii) have the highest ordering of all suitable follows (i.e. highest in the graph)
	
			if (desc->GetStructType() == Cond && desc->GetCondFollow() && desc->GetLoopHead() == header)
			{
				if (loopNodes[desc->GetCondFollow()->Order()])
				{
					// if the conditional's follow is in the same loop AND is lower in the loop, jump to this follow
					if (desc->Order() > desc->GetCondFollow()->Order())
						i = desc->GetCondFollow()->Order();
		
					// otherwise there is a backward jump somewhere to a node earlier in this loop. We don't need to
					// any nodes below this one as they will all have a conditional within the loop.
					else
						break;
				}

				else
				{
					// otherwise find the child (if any) of the conditional header that isn't inside the same
					// loop 
					CFGNode* succ = desc->GetOutEdges()[0];
					if (loopNodes[succ->Order()])
						if (!loopNodes[desc->GetOutEdges()[1]->Order()])
							succ = desc->GetOutEdges()[1];
						else
							succ = NULL;
						
					// if a potential follow was found, compare its ordering with the currently found follow
					if (succ && (!follow || succ->Order() > follow->Order()))
						follow = succ;
				}
			}
		} 
		#else
		{
			// using intervals, the follow is determined to be the child outside the loop of a
			// 2 way conditional header that is inside the loop such that it (the child) has
			// the highest order of all potential follows
			CFGNode* &desc = order[i];

			if (desc->GetStructType() == Cond && desc->GetCondType() != Case && loopNodes[desc->Order()])
			{
				for (int j = 0; j < desc->GetOutEdges().Size(); j++)
				{
					CFGNode* succ = desc->GetOutEdges()[j];

					// consider the current child 
					if (succ != header && !loopNodes[succ->Order()] && (!follow || succ->Order() > follow->Order()))
						follow = succ;
				}
			}
		}
		#endif

		// if a follow was found, assign it to be the follow of the loop under investigation
		if (follow)
			header->SetLoopFollow(follow);
	}
}

#ifndef INTERVALS

void TagNodesInLoop(CFGNode* header, NodePtrArr const &order, bool* &loopNodes)
// Pre: header has been detected as a loop header and has the details of the latching node
// Post: the nodes within the loop have been tagged
{
	assert(header->GetLatchNode());

	// traverse the ordering structure from the header to the latch node tagging the nodes
	// determined to be within the loop. These are nodes that satisfy the following:
	//  i) header.loopStamps encloses curNode.loopStamps and curNode.loopStamps encloses latch.loopStamps
	//  OR
	//  ii) latch.revLoopStamps encloses curNode.revLoopStamps and curNode.revLoopStamps encloses header.revLoopStamps
	//	OR
	//  iii) curNode is the latch node
	CFGNode const* latch = header->GetLatchNode();
	for (int i = header->Order() - 1; i >= latch->Order(); i--)
		if (order[i]->InLoop(header,latch))
		{
			// update the membership map to reflect that this node is within the loop
			loopNodes[i] = true;

			order[i]->SetLoopHead(header);
		}
#ifdef LOOPHEAD
	bool right = false;
	for (int i = 0; i < header->GetOutEdges().Size(); i++)
		right = right || (header->GetOutEdges()[i]->InLoop(header,latch));
	if (header != latch && !right)
			cout << "Header " << header->Order() << " has no succ's in the loop." << endl;
#endif

}

#else  // using intervals and derived sequences

void TagNodesInLoop(CFGNode* header, NodePtrArr const& order, bool* &intNodes, bool* &loopNodes)
// Pre: header has been detected as a loop header and has the details of the latching node
// Post: the nodes within the loop have been tagged (if they weren't already within a more
//       deeply nested loop) and are within the returned set of nodes
{
	// process all the nodes between the loop header and latch, tagging those that belong to the loop
	// and haven't already been tagged as belong to another loop
	for (int i = header->Order() - 1; i >= header->GetLatchNode()->Order(); i--)
	{
		CFGNode* curNode = order[i];

		if (intNodes[curNode->Order()])
		{
			// update the membership map to show that this node is within the loop
			loopNodes[curNode->Order()] = true;

			// tag it as belonging to this loop if it doesn't already belong to another one
			if (!curNode->GetLoopHead())
				curNode->SetLoopHead(header);
		}
	}
}
#endif


void Graphs::StructLoops(ProcHeader* curProc)
// Pre: The graph for curProc has been built.
// Post: Each node is tagged with the header of the most nested loop of which it is a member (possibly none).
// 		  The header of each loop stores information on the latching node as well as the type of loop it heads.
#ifndef INTERVALS
{
	// Process the nodes in order so that nesting is detected correctly.
	NodePtrArr const &order = curProc->Ordering;

	for (int i = order.Size() - 1; i >= 0; i--)
	{
		CFGNode* curNode = order[i];	// the current node under investigation
		CFGNode* latch = NULL;			// the latching node of the loop

		// If the current node has at least one back edge into it, it is a loop header. If there
		// are numerous back edges into the header, determine which one comes form the proper latching node
		// The proper latching node is defined to have the following properties:
		//		  i) has a back edge to the current node
		//		 ii) has the same case head as the current node
		//		iii) has the same loop head as the current node
		//		 iv) is not an nway node
		//		  v) is not the latch node of an enclosing loop
		//		 vi) has a lower ordering than all other suitable candiates
		// If no nodes meet the above criteria, then the current node is not a loop header
		NodePtrArr const &iEdges = curNode->GetInEdges();
		for (int j = 0; j < iEdges.Size(); j++)
		{
			CFGNode* pred = iEdges[j];
			if (pred->GetCaseHead() == curNode->GetCaseHead() &&							// ii)
				pred->GetLoopHead() == curNode->GetLoopHead() &&							// iii)
				(!latch || latch->Order() > pred->Order()) && 								// vi)
				!(pred->GetLoopHead() && pred->GetLoopHead()->GetLatchNode() == pred) &&	// v)
				pred->HasBackEdgeTo(curNode))												// i)

				latch = pred;
		}

		// if a latching node was found for the current node then it is a loop header. 
		if (latch)
		{
			// define the map that maps each node to whether or not it is within the current loop
			bool* loopNodes = new bool[curProc->size];
			for (int j = 0; j < curProc->size; j++)
				loopNodes[j] = false;

			curNode->SetLatchNode(latch);

			// the latching node may already have been structured as a conditional header. If it is not
			// also the loop header (i.e. the loop is over more than one block) then reset
			// it to be a sequential node otherwise it will be correctly set as a loop header only later
			if (latch != curNode && latch->GetStructType() == Cond)
				latch->SetStructType(Seq);
	
			// set the structured type of this node
			curNode->SetStructType(Loop);

			// tag the members of this loop
			TagNodesInLoop(curNode,curProc->Ordering, loopNodes);

			// calculate the type of this loop
			DetermineLoopType(curNode, loopNodes);

			// calculate the follow node of this loop
			FindLoopFollow(curNode,curProc->Ordering, loopNodes);

			// delete the space taken by the loopnodes map
			delete[] loopNodes;
		}
	}
#else
{
	// process the derived graphs of the current procedure
	for (int gLevel = 0; gLevel < curProc->derivedGraphs.Size(); gLevel++)
	{
		DerivedGraph* curGraph = curProc->derivedGraphs[gLevel];
		
		// process each of the intervals in the current derived graph
		for (int j = 0; j < curGraph->intervals.Size(); j++)
		{
			IntNode* curInt = static_cast<IntNode*>(curGraph->intervals[j]);
			CFGNode* latch = NULL;
			
			// find the G0 basic block node at the head of this interval
			CFGNode* headNode = curInt;
			for (int k = 0; k <= gLevel; k++)
				headNode = static_cast<IntNode*>(headNode)->Nodes()[0];

			// find the cfg nodes that belong in the current interval.
			bool* cfgNodes = new bool[curProc->size];
			for (int i = 0; i < curProc->size; i++)
				cfgNodes[i] = false;

			curInt->FindNodesInInt(cfgNodes,gLevel);
#ifdef TESTINTS
			cerr << "BB nodes in interval " << curInt->Ident() << ": ";
			for (int h = 0; h < curProc->size; h++)
				if (cfgNodes[h])
					cerr << h << ", ";
			cerr << endl;
#endif
			
			// find the greatest enclosing back edge (if any) from a node within the same interval
			for (int k = 0; k < headNode->GetInEdges().Size(); k++)
			{
				CFGNode* pred = headNode->GetInEdges()[k];
				if (pred->HasBackEdgeTo(headNode) && cfgNodes[pred->Order()] &&
					(!latch || latch->Order() > pred->Order()))
					latch = pred;
			}
			
			// if this is a loop header (i.e. a latch was found) then tag the nodes in the loop
			// and determine its type
			if (latch)
				// ensure that the latch does not belong to another loop
				if (!latch->GetLoopHead())
				{
					// define the map that maps each node to whether or not it is within the current loop
					bool* loopNodes = new bool[curProc->size];
					for (int i = 0; i < curProc->size; i++)
						loopNodes[i] = false;

					// if the head node has already been determined as a loop header then the nodes
					// within this loop have to be untagged and the latch reset to its original type
					if (headNode->GetLatchNode())
					{
						//CFGNode* oldLatch = static_cast<CFGNode*>(headNode->GetLatchNode());
						CFGNode* oldLatch = (CFGNode*)(headNode->GetLatchNode());

						// reset the latch node's structured class. Only need to do this for a 2 way latch
						if (oldLatch->GetType() == cBranch)
							oldLatch->SetStructType(Cond);

						// untag the nodes
						for (int i = headNode->Order() - 1; i >= oldLatch->Order(); i--)
							if (curProc->Ordering[i]->GetLoopHead() == headNode)
								curProc->Ordering[i]->SetLoopHead(NULL);
					}			

					headNode->SetLatchNode(latch);

					// the latching node will already have been structured as a conditional header. If it is not
					// also the loop header (i.e. the loop is over more than one block) then reset
					// it to be a sequential node otherwise it will be correctly set as a loop header only later
					if (latch != headNode)
						latch->SetStructType(Seq);
		
					// set the structured type of this node
					headNode->SetStructType(Loop);				

					// Tag the nodes within the loop
					TagNodesInLoop(headNode,curProc->Ordering, cfgNodes,loopNodes);

					// calculate the type of this loop
					DetermineLoopType(headNode);

					// calculate the follow node of this loop
					FindLoopFollow(headNode,curProc->Ordering, loopNodes);

					// delete the space taken by the loopnodes map
					delete[] loopNodes;
				}
			delete[] cfgNodes;
		}
	}
#endif

#ifdef TESTLOOPS
	cerr << "\nLoop info for procedure " << curProc->name << endl;
   cerr << "Node\t| LoopHead" << endl;
	NodePtrArr const &order = curProc->Ordering;
   for (int i = 0; i < order.Size(); i++)
   {
      CFGNode* curNode = order[i];
		cerr << curNode->Order() << "\t|  ";
		if (curNode->GetLoopHead())
		{
			cerr << curNode->GetLoopHead()->Order();
			if (curNode->GetLatchNode())
			{
				cerr << "  (loop header) Type: " << curNode->GetLoopType() << ", Follow: ";
				if (curNode->GetLoopFollow())
					cerr << curNode->GetLoopFollow()->Order();
				else
					cerr << "(no follow)";
			}
			cerr << endl;
		}
		else
			cerr << "-" << endl;
   }
#endif
}

//********************************************************************************
// Post-processing structuring routines
//********************************************************************************
void Graphs::CheckConds(ProcHeader* curProc)
// This routine is called after all the other structuring has been done. It detects
// conditionals that are in fact the head of a jump into/outof a loop or into a case body. 
// Only forward jumps are considered as unstructured backward jumps will always be generated nicely.
{
	for (int i = 0; i < curProc->size; i++)
	{
		CFGNode* curNode = curProc->Ordering[i];
		NodePtrArr const &oEdges = curNode->GetOutEdges();
		
		// consider only conditional headers that have a follow and aren't case headers
		if ((curNode->GetStructType() == Cond || curNode->GetStructType() == LoopCond) &&
				curNode->GetCondFollow() && curNode->GetCondType() != Case)
		{
			// define convenient aliases for the relevant loop and case heads and the out edges
			CFGNode const* myLoopHead = (curNode->GetStructType() == LoopCond ? curNode : curNode->GetLoopHead());
			CFGNode const* follLoopHead = curNode->GetCondFollow()->GetLoopHead();

			// analyse whether this is a jump into/outof a loop
			if (myLoopHead != follLoopHead) 
			{
				// we want to find the branch that the latch node is on for a jump
				// out of a loop
				if (myLoopHead)
				{
					CFGNode const* myLoopLatch = myLoopHead->GetLatchNode();

					// does the then branch goto the loop latch?
					if (oEdges[THEN]->IsAncestorOf(myLoopLatch) || oEdges[THEN] == myLoopLatch)
					{
						curNode->SetUnstructType(JumpInOutLoop);
						curNode->SetCondType(IfElse);
					}
					// does the else branch goto the loop latch?
					else if (oEdges[ELSE]->IsAncestorOf(myLoopLatch) || oEdges[ELSE] == myLoopLatch)
					{
						curNode->SetUnstructType(JumpInOutLoop);
						curNode->SetCondType(IfThen);
					}
				}

				if(curNode->GetUnstructType() == Structured && follLoopHead)
				// find the branch that the loop head is on for a jump into a loop body. If a branch has
				// already been found, then it will match this one anyway
				{  
					// does the else branch goto the loop head?
					if (oEdges[THEN]->IsAncestorOf(follLoopHead) || oEdges[THEN] == follLoopHead)
					{
						curNode->SetUnstructType(JumpInOutLoop);
						curNode->SetCondType(IfElse);
					}
					// does the else branch goto the loop head?
					else if (oEdges[ELSE]->IsAncestorOf(follLoopHead) || oEdges[ELSE] == follLoopHead)
					{
						curNode->SetUnstructType(JumpInOutLoop);
						curNode->SetCondType(IfThen);
					}
				}
			}

			// this is a jump into a case body if either of its children don't have the same
			// same case header as itself
			if (curNode->GetUnstructType() == Structured &&
				 (curNode->GetCaseHead() != curNode->GetOutEdges()[THEN]->GetCaseHead() ||
				  curNode->GetCaseHead() != curNode->GetOutEdges()[ELSE]->GetCaseHead()))
			{
				CFGNode const* myCaseHead = curNode->GetCaseHead();
				CFGNode const* thenCaseHead = curNode->GetOutEdges()[THEN]->GetCaseHead();
				CFGNode const* elseCaseHead = curNode->GetOutEdges()[ELSE]->GetCaseHead();
				if (thenCaseHead == myCaseHead && (!myCaseHead || elseCaseHead != myCaseHead->GetCondFollow()))
				{
					curNode->SetUnstructType(JumpIntoCase);
					curNode->SetCondType(IfElse);
				}
				else if (elseCaseHead == myCaseHead && (!myCaseHead || thenCaseHead != myCaseHead->GetCondFollow()))
				{
					curNode->SetUnstructType(JumpIntoCase);
					curNode->SetCondType(IfThen);
				}
			}	
		}

		// for 2 way conditional headers that don't have a follow (i.e. are the source of a back
		// edge) and haven't been structured as latching nodes, set their follow to be the
		// non-back edge child.
		if (curNode->GetStructType() == Cond && !curNode->GetCondFollow() &&
			 curNode->GetUnstructType() == Structured && curNode->GetCondType() != Case)
		{
			// latching nodes will already have been reset to Seq structured type
			assert(HasABackEdge(curNode));

			if (curNode->HasBackEdgeTo(curNode->GetOutEdges()[THEN]))
			{
				curNode->SetCondType(IfThen);
				curNode->SetCondFollow(curNode->GetOutEdges()[ELSE]);
			}
			else
			{
				curNode->SetCondType(IfElse);
				curNode->SetCondFollow(curNode->GetOutEdges()[THEN]);
			}
		}

	}
}
//********************************************************************************
// Complete structuring routine
//********************************************************************************
void Graphs::Structure()
{
#ifdef GETSTATS
	double t[3] = {0,0,0};
	dtime(t);
	stats.structMemCost  = MemStats[2];
	stats.structMemAlloc = MemStats[0];
#endif

	ProcHeader* curProc;

	for (curProc = procs; curProc; curProc = curProc->next)
	{
		FindImmedPDom(curProc);
		StructConds(curProc);
		StructLoops(curProc);
		CheckConds(curProc);
	}

#ifdef GETSTATS
	dtime(t);
	stats.structTime = t[1];
	stats.structMemCost  = MemStats[2] - stats.structMemCost;
	stats.structMemAlloc = MemStats[0] - stats.structMemAlloc;
#endif
}
