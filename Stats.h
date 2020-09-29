/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File: Stats.h
// Purpose: provides a data structure that can be used to store
//		relevant statistics about the structuring process

#ifndef _STATS_
#define _STATS_

/*****************************************************/
/*  UNIX dtime(). This is the preferred UNIX timer.  */
/*  Provided by: Markku Kolkka, mk59200@cc.tut.fi    */
/*  HP-UX Addition by: Bo Thide', bt@irfu.se         */  
/*****************************************************/
int dtime(double p[]);

struct Stats {
	int numAsmIns;			// number of assembly instructions
	int numGraphNodes;		// number of basic blocks/graph nodes
	int numGraphEdges;		// number of graph edges
	int numUnreachIns;		// number of unreachable nodes removed
	int numGotos;				// number of gotos generated
	int numLoops;
	int num2ways;
	int numNways;
	int numContBrks;		// number of continue's or break's from a loop
	int maxIndent;			// maximum indentation level reached
	
#ifdef INTERVALS
	int numIntervals;		// number of intervals in all derived graphs
	int numDerGraphs;		// number of graphs in derived sequence
	double bldDerSeqTime;	// time to build the derived sequence of graphs
	int derGraphsMem;		// amount of memory taken up by the derived graphs

	int derSeqMemCost;		// memory usage added by the derived sequences
	int derSeqMemAlloc;		// memory allocated during building of derived
							// sequences
#endif

	int structMemCost;		// memory usage added during structuring
	int structMemAlloc;		// memory allocated during strucuring
	double structTime;		// time to do the structuring
	double codeGenTime;		// time to generate the code

	//constructor function just sets everything to zero
	Stats() {
		numAsmIns = numGraphNodes = numGraphEdges = 
		numUnreachIns = numGotos = numLoops =
		num2ways = numNways = numContBrks = maxIndent =
#ifdef INTERVALS
		numIntervals = 
		derSeqMemCost = derSeqMemAlloc =
#endif
		structMemCost = structMemAlloc = 0;
		structTime = codeGenTime =
#ifdef INTERVALS
		bldDerSeqTime =
#endif
		0.0;
	}
};

extern Stats stats;

#endif
