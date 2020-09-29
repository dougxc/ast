/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: Options.cpp
//Author: Doug Simon
//Purpose: provides a class to store and process the command line arguments

#ifndef _OPTIONS_
#define _OPTIONS_

// define a structure to store the command line options
class Options {
public:
	bool			genCode;			// generate the high level code
	bool			genDotty;		// generate the graphviz output
	bool			structInfo;		// graphviz node shows structured info for each node
	bool			immPDom;			// graphviz node shows immediate post dominator info
	bool			removeGotos;	// preform the goto removal step
	bool			showHeads;		// show the headers of the relevant structures of which
										// a node is a member
	bool			revOrder;		// show the order of this node within the reverse graph
	bool			blocksOnly;		// the generated code only includes basic blocks and
										// control flow statements

	// extracts the command line arguments
	char* InitArgs(int argc, char *argv[]);
};

#endif
