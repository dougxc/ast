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
//Purpose: implements the Options class
// 11 Feb 98 - Cristina
//	added genCode = true when blocksOnly = true so that code is generated.

#include <iostream.h>
#include <stdlib.h>
#include "Options.h"


char* Options::InitArgs(int argc, char *argv[])
{
	char *pc;
	char *progname = *argv;

	// Initialise all the options to false
	structInfo  = false;
	immPDom     = false;
	removeGotos = false;
	showHeads   = false;
	revOrder    = false;
	genCode     = false;
	genDotty    = false;
	blocksOnly  = false;
	
	while (--argc > 0 && (*++argv)[0] == '-')
		for (pc = argv[0] + 1; *pc; pc++)
			switch (*pc) {
			case 'c':
				genCode = true;
				break;
			case 'd':
				genDotty = true;
				break;
			case 's':
				structInfo = true;
				genDotty = true;
				break;
			case 'p':
				immPDom = true;
				genDotty = true;
				break;
			case 'g':
				removeGotos = true;
				break;
			case 'h':
				showHeads = true;
				genDotty = true;
				break;
			case 'r':
				revOrder = true;
				genDotty = true;
				break;
			case 'b':
				blocksOnly = true;
				genCode = true;
				break;
			default:
				cerr << " Bad command line argument: " << *pc << endl;
				cerr << " Run the program without any arguments to see the available options." << endl;
				exit(1);
				break;
			}	

	// return the filename
	if (argc == 1)
		return *argv;
	else
	{
		cerr << "Usage: " << progname << " [-cgdsphr] Sparc_asm_file" << endl;
		cerr << "\t-c generate the high level code" << endl;
		cerr << "\t-g remove unecessary goto's from the generated code";
		cerr << " (currently unavailable)" << endl;
		cerr << "\t-d generate the graphviz output" << endl;
		cerr << endl;
		cerr << "\tThe following option implies -c" << endl;
		cerr << endl;
		cerr << "\t-b high level code only contains blocks and control flow statements" << endl;
		cerr << endl;
		cerr << "\tThe remaining options all imply -d" << endl;
		cerr << endl;
		cerr << "\t-s show structuring info" << endl;
		cerr << "\t-p show immediate post dominators" << endl; 
		cerr << "\t-h show head of stucture enclosing each node" << endl;
		cerr << "\t-r show the order of each node within the reverse graph" << endl;
		cerr << endl;
		exit(1);
	}
}

