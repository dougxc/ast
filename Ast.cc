/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: Ast.cpp
//Author: Doug Simon
//Purpose: this is the main driver for an assembly structuring tool. It calls
//	various other modules to transform an SPARC8 assembly program (as
//	generated by 'gcc -S' or 'gcc -O3 -S') into a structure control flow
//	graph (CFG) and a structured assembly listing.


#include <stdlib.h>
#include "Graphs.h"
#include "Source.h"
#include "Options.h"
#include "Stats.h"
#include "MemAdvise.h"

// define global variables to store the command line options and
// the runtime statistics
Options options;
Stats stats;
int MemStats[3] = {0,0,0};

main(int argc, char *argv[])
{
	Source assCode;
	Graphs cfgs;
	char* filename;

	// extract the command line arguments
	filename = options.InitArgs(argc, argv);

	//Read in the assembly source and transform it into an array
	//of instructions
	assCode.Build(filename);

	//Build the list of basic blocks from these instructions
	cfgs.BuildNodes(assCode);

	//Add the graph edges to these basic blocks
	cfgs.DefineEdges();

	//Build the list of CFG's information nodes for each procedure
	cfgs.DefineCfgs();

	//Do the dfs labelling of each node
	cfgs.SetTimeStamps();

#ifdef INTERVALS
	// Build the sequence of derived graphs for each CFG
	cfgs.BuildDerivedSequences();

	// Display the sequence of derived graphs for each CFG
//	cfgs.DisplayDerivedSequences();
#endif

	//Apply the structuring algorithm to the CFG's of the program
	cfgs.Structure();

	if (options.genCode)
	{
		//Generate HLL code
		cfgs.CodeGen(filename);
	}

	if (options.genDotty)
		//Generate the graphviz input file
		cfgs.GenerateGraphvizFile(filename);

#ifdef GETSTATS
	// display the relevant stats
	// Note: the graphs stats are for all the cfgs within a program and are
	// gathered for the graphs after the unreachable nodes have been removed.
	cout << filename << ":";
#ifdef INTERVALS
	cout << "(derived sequence algorithm)" << endl;
#else
	cout << "(parenthesis theory algorithm)" << endl;
#endif 
	cout << "\t# assembly instructions in input = " << stats.numAsmIns << endl;
	cout << "\t# unreachable instructions = " << stats.numUnreachIns << endl;
	cout << "\t# graph nodes = " << stats.numGraphNodes << endl;
	cout << "\t# graph edges = " << stats.numGraphEdges << endl;
#ifdef INTERVALS
	cout << "\t# intervals = " << stats.numIntervals << endl;
	cout << "\t# derived graphs = " << stats.numDerGraphs << endl;
	cout << "\t time to build derived sequences (DS) = " << stats.bldDerSeqTime << endl;
#else
#endif
	cout << "\t time to structure CFG's = " << stats.structTime << endl;
#ifdef INTERVALS
	cout << "\t memory for DS = " << stats.derSeqMemCost << endl;
//	cout << "\t memory used building DS = " << stats.derSeqMemAlloc << endl;
#endif
//	cout << "\t memory added by structuring = " << stats.structMemCost << endl;
//	cout << "\t memory used while structuring = " << stats.structMemAlloc << endl;
	if (options.genCode)
	{
		cout << "\t time to generate HLL code = " << stats.codeGenTime << endl;
		cout << "\t number of goto's generated = " << stats.numGotos << endl;
		cout << "\t number of loops's generated = " << stats.numLoops << endl;
		cout << "\t number of if-then-{else}'s generated = " << stats.num2ways << endl;
		cout << "\t number of switch's generated = " << stats.numNways << endl;
		cout << "\t number of loop continue or break statements generated = " << stats.numContBrks << endl;
	}
#endif
}
