/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: NodeCodeGen.cpp
//Author: Doug Simon
//Purpose: provides the operations to generate code for a CFG node that has been
//		analysed by the structuring algorithms.

#include <math.h>
#include <stdio.h>
#include "Graphs.h"
#include "Node.h"
#include "TypeDefs.h"
#include "StringFunctions.h"
#include "Options.h"
#include "Stats.h"

extern Options options;

// some constants required to indicate extra code to be added
#define CLOSE_BRACKET 2		// length of "}\n"
#define DO_HEADER 5			// length of "do {\n"
#define ENDLESS_HEADER 11	// langth of "for (;;) {\n"

char* Indent(int indLevel, int extra = 0)
// Return the string containing indLevel tabstops. 
// If extra = CLOSE_BRACKET then append "}\n" to the returned string
// If extra = DO_HEADER then append "do {\n" to the returned string
{
	char* retStr = new char[indLevel + extra + 1];
	memset(retStr,'\t',indLevel);

	retStr[indLevel] = '\0';

	// add the extra stuff in necessary
	if (extra == CLOSE_BRACKET)
		strcat(retStr,"}\n");
	else if (extra == DO_HEADER)
		strcat(retStr,"do {\n");
	else if (extra == ENDLESS_HEADER)
		strcat(retStr,"for (;;) {\n");

	return retStr;
}

bool CFGNode::AllParentsGenerated() const
// Return true if every parent (i.e. forward in edge source) of this node has had its code generated
{
	for (int i = 0; i < inEdges.Size(); i++)
		if (!inEdges[i]->HasBackEdgeTo(this) && inEdges[i]->traversed != DFS_CODEGEN)
			return false;
	return true;
}

void CFGNode::EmitGotoAndLabel(StrArr &HLLCode, int indLevel, CFGNode* dest)
// Emits a goto statement (at the correct indentation level) with the destination label for dest.
// Also places the label just before the destination code if it isn't already there.
// If the goto is to the return block, emit a 'return' instead.
// Also, 'continue' and 'break' statements are used instead if possible
{
	// is this a goto to the ret block?
	if (dest->type == ret && dest->instructs.Size() == 2)
	{
		// get the delayed instruction from the return block which will always be the second and last
		// instruction in the block
		char const* delayedIns = dest->instructs[1]->GetString();
		char* retStmt = new char[indLevel * 2 + strlen("return;\n\n") + strlen(delayedIns) + 1];

		sprintf(retStmt,"%s%s\n%sreturn;\n",Indent(indLevel),delayedIns,Indent(indLevel));
		HLLCode.Add(retStmt);
	}
	else
	{
		char* gotoStmt;

		if (loopHead && (loopHead == dest || loopHead->loopFollow == dest))
		{
			gotoStmt = new char[indLevel + strlen("continue;\n") + 1];
			sprintf(gotoStmt,"%s%s\n",Indent(indLevel),(loopHead == dest ? "continue;" : "break;"));
#ifdef GETSTATS
			stats.numContBrks++;
#endif
		}
		else
		{
 			gotoStmt = new char[indLevel + strlen("goto L;\n") + 
								(dest->ord == 0 ? 1 : static_cast<int>(ceil(log10(dest->ord)))) + 1];
			sprintf(gotoStmt,"%sgoto L%d;\n",Indent(indLevel),dest->ord);

			// don't emit the label if it already has been emitted or the code 
			// for the destination has not yet been generated
			if (!dest->hllLabel && dest->traversed == DFS_CODEGEN)
				sprintf(dest->labelStr,"L%d:\n",dest->ord);

			dest->hllLabel = true;
#ifdef GETSTATS
			stats.numGotos++;
#endif
		}
		HLLCode.Add(gotoStmt);
	}
}

//************************************************************************
// Generate code for body of a basic block
//************************************************************************
void CFGNode::WriteBB(StrArr &HLLCode, int indLevel)
// Generates code for each non CTI (except procedure calls) statement within the block.
{
	// allocate space for a label to be generated for this node and add this to
	// the generated code. The actual label can then be generated now or back patched later
	int labelSize = (ord == 0 ? 1 : static_cast<int>(ceil(log10(ord)))) + strlen("L:\n") + 1;
	labelStr = new char[labelSize];
	HLLCode.Add(labelStr);
	if (hllLabel)
		sprintf(labelStr,"L%d:\n",ord);
	else
		strcpy(labelStr,"");
#ifdef GETSTATS
	stats.maxIndent = (stats.maxIndent < indLevel ? indLevel : stats.maxIndent);
#endif
	
	if (options.blocksOnly)
	{
		char* codeString = new char[indLevel + 20];
		sprintf(codeString,"%sBB%d;\n",Indent(indLevel),ord);
		HLLCode.Add(codeString);
	}
	else
	{
		// allocate the space required by all the non-procedure call, non-CTI's in the block
		char* codeString = new char[indLevel * instructs.Size() + InsSpace()];

		// initialise the string
		codeString[0] = '\0';

		for (int i = 0; i < instructs.Size(); i++)
			// if this is the 2nd last instruction in a block delimited by a non-procedure
			// call CTI, then don't print out this CTI
			if (!(i == instructs.Size() - 2 && GetCTI() && type != call))
				sprintf(codeString,"%s%s%s\n",codeString,Indent(indLevel),instructs[i]->GetString());
		
		// add the code for this block to the code for the procedure
		HLLCode.Add(codeString);	
	}

	// save the indentation level that this node was written at
	indentLevel = indLevel;
}

//*********************************************************************
// Generate code for control flow info for each basic block
//*********************************************************************
void CFGNode::WriteCode(StrArr &HLLCode, int indLevel, CFGNode const* latch, NodePtrArr &followSet, NodePtrArr &gotoSet)
{
	// If this is the follow for the most nested enclosing conditional, then
	// don't generate anything. Otherwise if it is in the follow set
	// generate a goto to the follow
	CFGNode* enclFollow = static_cast<CFGNode*>(followSet.Size() == 0 ? NULL : followSet[followSet.Size() - 1]);

	if (gotoSet.IsIn(this) && !IsLatchNode() && ((latch && this == latch->loopHead->loopFollow) || !AllParentsGenerated()))
	{
		EmitGotoAndLabel(HLLCode, indLevel, this);
		return;
	}
	else if (followSet.IsIn(this))
	{
		if (this != enclFollow)
		{
			EmitGotoAndLabel(HLLCode, indLevel, this);
			return;
		}
		else
			return;
	}

	// Has this node already been generated?
	if (traversed == DFS_CODEGEN)
	{
		// this should only occur for a loop over a single block
		assert(sType == Loop && lType == PostTested && latchNode == this);
		return;
	}
	else
		traversed = DFS_CODEGEN;

	// if this is a latchNode and the current indentation level is
	// the same as the first node in the loop, then this write out its body and return
	// otherwise generate a goto
	if (IsLatchNode())
		if (indLevel == latch->loopHead->indentLevel + (latch->loopHead->lType == PreTested ? 1 : 0))
		{
			WriteBB(HLLCode, indLevel);
			return;
		}
		else
		{
			// unset its traversed flag
			traversed = UNTRAVERSED;

			EmitGotoAndLabel(HLLCode, indLevel, this);
			return;
		}
	
	// declare some strings that can't be initialised in the body of the switch
	char* condPred;
	char* predString;
	char* opCode;
	char* retStmt;

	switch(sType) {
	case Loop:
	case LoopCond:

		// add the follow of the loop (if it exists) to the follow set
		if (loopFollow)
			followSet.Add(loopFollow);

		if (lType == PreTested)
		{
			assert(latchNode->outEdges.Size() == 1);

			// write the body of the block (excluding the predicate)
			WriteBB(HLLCode, indLevel);

			// write the 'while' predicate
			//opCode = static_cast<char*>(Type2String(GetCTI()->GetType()));
			opCode = (char*)(Type2String(GetCTI()->GetType()));
			predString = new char[(indLevel * 2) + 1 + strlen("while (") + MAX_OPCODE_LEN + strlen(")\n") + strlen("{\n") + 1];
			sprintf(predString,"%swhile (%s%s)\n%s{\n",Indent(indLevel),(outEdges[THEN] == loopFollow ? "!" : ""),opCode,Indent(indLevel));
			HLLCode.Add(predString);

#ifdef GETSTATS
			stats.numLoops++;
#endif

			// write the code for the body of the loop
			CFGNode* loopBody = (outEdges[ELSE] == loopFollow) ? outEdges[THEN] : outEdges[ELSE];
			loopBody->WriteCode(HLLCode, indLevel + 1, latchNode, followSet, gotoSet);

			// if code has not been generated for the latch node, generate it now
			if (latchNode->traversed != DFS_CODEGEN)
			{
				latchNode->traversed = DFS_CODEGEN;
				latchNode->WriteBB(HLLCode, indLevel+1);
			}

			// rewrite the body of the block (excluding the predicate) at the next nesting level
			// after making sure another label won't be generated
			hllLabel = false;
			WriteBB(HLLCode, indLevel+1);

			// write the loop tail
			HLLCode.Add(Indent(indLevel,CLOSE_BRACKET));
		}
		else 
		{

			// write the loop header
			if (lType == Endless)
				HLLCode.Add(Indent(indLevel,ENDLESS_HEADER));
			else
				HLLCode.Add(Indent(indLevel,DO_HEADER));
#ifdef GETSTATS
			stats.numLoops++;
#endif

			// if this is also a conditional header, then generate code for the
			// conditional. Otherwise generate code for the loop body.
			if (sType == LoopCond)
			{
				// set the necessary flags so that WriteCode can successfully be called
				// again on this node
				sType = Cond;
				traversed = UNTRAVERSED;
				
				WriteCode(HLLCode, indLevel + 1, latchNode, followSet, gotoSet);
			}
			else
			{
				WriteBB(HLLCode, indLevel+1);

				// write the code for the body of the loop
				outEdges[0]->WriteCode(HLLCode, indLevel + 1, latchNode, followSet, gotoSet);
			}

			if (lType == PostTested)
			{

				// if code has not been generated for the latch node, generate it now
				if (latchNode->traversed != DFS_CODEGEN)
				{
					latchNode->traversed = DFS_CODEGEN;
					latchNode->WriteBB(HLLCode, indLevel+1);
				}
			
				// string for the repeat loop predicate.
				predString = new char[indLevel + strlen("} while (") + MAX_OPCODE_LEN + strlen(")\n") + 2];

				// write the repeat loop predicate
				//opCode = static_cast<char*>(Type2String(latchNode->GetCTI()->GetType()));
				opCode = (char*)(Type2String(latchNode->GetCTI()->GetType()));
				sprintf(predString,"%s} while (%s);\n",Indent(indLevel),opCode);
				HLLCode.Add(predString);
			}
			else
			{
				assert(lType == Endless);

				// if code has not been generated for the latch node, generate it now
				if (latchNode->traversed != DFS_CODEGEN)
				{
					latchNode->traversed = DFS_CODEGEN;
					latchNode->WriteBB(HLLCode, indLevel+1);
				}

				// write the closing bracket for an endless loop
				HLLCode.Add(Indent(indLevel,CLOSE_BRACKET));
			}
		}

		// write the code for the follow of the loop (if it exists)
		if (loopFollow)
		{
			// remove the follow from the follow set
			followSet.RemoveLast();

			if (loopFollow->traversed != DFS_CODEGEN)
				loopFollow->WriteCode(HLLCode, indLevel, latch, followSet, gotoSet);
			else
				EmitGotoAndLabel(HLLCode, indLevel,loopFollow);
		}
		break;

	case Cond:

		// reset this back to LoopCond if it was originally of this type
		if (latchNode)
			sType = LoopCond;

		// for 2 way conditional headers that are effectively jumps into or out of a
		// loop or case body, we will need a new follow node
		CFGNode* tmpCondFollow;
		tmpCondFollow = NULL;

		// keep track of how many nodes were added to the goto set so that the correct number
		// are removed
		int gotoTotal;
		gotoTotal = 0;

		// add the follow to the follow set if this is a case header
		if (cType == Case)
			followSet.Add(condFollow);
		
		else if (cType != Case && condFollow)
		{
			// For a structured two conditional header, its follow is added to the follow set
			CFGNode* myLoopHead;
			myLoopHead = (sType == LoopCond ? this : loopHead);

			if (usType == Structured)
				followSet.Add(condFollow);
	
			// Otherwise, for a jump into/outof a loop body, the follow is added to the goto set.
			// The temporary follow is set for any unstructured conditional header
			// branch that is within the same loop and case.
			else 
			{
				if (usType == JumpInOutLoop)
				{
					// define the loop header to be compared against
					CFGNode* myLoopHead = (sType == LoopCond ? this : loopHead);

					gotoSet.Add(condFollow);
					gotoTotal++;
	
					// also add the current latch node, and the loop header of the follow if they exist
					if (latch)
					{
						//gotoSet.Add(static_cast<CFGNode*>(latch));
						gotoSet.Add((CFGNode*)(latch));
						gotoTotal++;
					}
					if (condFollow->loopHead && condFollow->loopHead != myLoopHead)
					{
						gotoSet.Add(condFollow->loopHead);
						gotoTotal++;
					}
				}

				if (cType == IfThen)
					tmpCondFollow = outEdges[ELSE];
				else
					tmpCondFollow = outEdges[THEN];

				// for a jump into a case, the temp follow is added to the follow set
				if (usType == JumpIntoCase)
					followSet.Add(tmpCondFollow);
			}
		}

		// write the body of the block (excluding the predicate)
		WriteBB(HLLCode, indLevel);

		// write the conditional header 
		if (cType == Case)
		{
			condPred = new char[indLevel * 2 + strlen("switch (!") + MAX_OPCODE_LEN + strlen(") {\n") + 1];
			sprintf(condPred,"%sswitch (%s) {\n",Indent(indLevel),"Reg0");
		}
		else
		{
			//opCode = static_cast<char*>(Type2String(GetCTI()->GetType()));
			opCode = (char*)(Type2String(GetCTI()->GetType()));
			condPred = new char[indLevel * 2 + strlen("switch (!") + MAX_OPCODE_LEN + strlen(") {\n") + 1];
			sprintf(condPred,"%sif (%s%s) {\n",Indent(indLevel),(cType == IfElse ? "!" : ""),opCode);
		}
		HLLCode.Add(condPred);

		// write code for the body of the conditional
		if (cType != Case)
		{

#ifdef GETSTATS
			stats.num2ways++;
#endif

			CFGNode* succ = (cType == IfElse ? outEdges[ELSE] : outEdges[THEN]);

			// emit a goto statement if the first clause has already been generated or it
			// is the follow of this node's enclosing loop
			if (succ->traversed == DFS_CODEGEN || (loopHead && succ == loopHead->loopFollow))
				EmitGotoAndLabel(HLLCode, indLevel + 1, succ);
			else	
				succ->WriteCode(HLLCode, indLevel + 1, latch, followSet, gotoSet);

			// generate the else clause if necessary
			if (cType == IfThenElse)
			{
				// generate the 'else' keyword and matching brackets
				char* elseStr = new char[indLevel * 2 + strlen("} else\n{\n") + 1];
				sprintf(elseStr,"%s} else\n%s{\n",Indent(indLevel),Indent(indLevel));
				HLLCode.Add(elseStr);

				succ = outEdges[ELSE];

				// emit a goto statement if the second clause has already been generated
				if (succ->traversed == DFS_CODEGEN)
					 EmitGotoAndLabel(HLLCode, indLevel + 1, succ);
				else
					succ->WriteCode(HLLCode, indLevel + 1, latch, followSet, gotoSet);
			}	
		}
		else		// case header
		{

#ifdef GETSTATS
			stats.numNways++;
#endif

			// generate code for each out branch
			for (int i = 0; i < outEdges.Size(); i++)
			{
				// emit a case label
				char* caseStr = new char[indLevel + strlen("cond_:\n") + (i == 0 ? 1 : static_cast<int>(ceil(log10(i)))) + 1];
				sprintf(caseStr,"%scase cond_%d:\n",Indent(indLevel),i);
				HLLCode.Add(caseStr);

				// generate code for the current outedge
				CFGNode* succ = outEdges[i];
//				assert(succ->caseHead == this || succ == condFollow || HasBackEdgeTo(succ));
				if (succ->traversed == DFS_CODEGEN)
					EmitGotoAndLabel(HLLCode, indLevel + 1, succ);
				else
					succ->WriteCode(HLLCode, indLevel + 1, latch, followSet, gotoSet);

				// generate the 'break' statement
				caseStr = new char[indLevel + 1 + strlen("break;\n") + 1];
				sprintf(caseStr,"%sbreak;\n",Indent(indLevel + 1));
				HLLCode.Add(caseStr);
			}
		}

		// generate the closing bracket
		HLLCode.Add(Indent(indLevel,CLOSE_BRACKET));

		// do all the follow stuff if this conditional had one
		if (condFollow)
		{
			// remove the original follow from the follow set if it was added by this header
			if (usType == Structured || usType == JumpIntoCase)
			{
				assert(gotoTotal == 0);
				followSet.RemoveLast();
			}

			// else remove all the nodes added to the goto set
			else
				for (int i = 0; i < gotoTotal; i++)
					gotoSet.RemoveLast();

			// do the code generation (or goto emitting) for the new conditional follow if it exists
			// otherwise do it for the original follow
			if (!tmpCondFollow)
				tmpCondFollow = condFollow;
			
			if (tmpCondFollow->traversed == DFS_CODEGEN)
				EmitGotoAndLabel(HLLCode, indLevel, tmpCondFollow);
			else
				tmpCondFollow->WriteCode(HLLCode, indLevel,latch,followSet, gotoSet);
		}

		break;
	
	case Seq:
		// generate code for the body of this block
		WriteBB(HLLCode, indLevel);

		// return if this is the 'return' block (i.e. has no out edges) after emmitting a 'return' statement
		if (type == ret)
		{
			retStmt = new char[indLevel + strlen("return;\n") + 1];
			sprintf(retStmt,"%sreturn;\n",Indent(indLevel));
			HLLCode.Add(retStmt);
			return;
		}

		// generate code for its successor if it hasn't already been visited and is in the same loop/case
		// and is not the latch for the current most enclosing loop. 
		// The only exception for generating it when it is not in the same loop
		// is when this when it is only reached from this node
		CFGNode* child = outEdges[0];
		if (child->traversed == DFS_CODEGEN || ((child->loopHead != loopHead) && 
			(!child->AllParentsGenerated() || followSet.IsIn(child))) ||
			(latch && latch->loopHead->loopFollow == child) ||
		!(caseHead == child->caseHead || (caseHead && child == caseHead->condFollow)))

			EmitGotoAndLabel(HLLCode, indLevel, outEdges[0]);
		else
			outEdges[0]->WriteCode(HLLCode, indLevel,latch,followSet, gotoSet);

		break;
	}
}
