# 
#  Copyright (C) 1997, Doug Simon
# 
#  See the file "LICENSE.TERMS" for information on usage and
#  redistribution of this file, and for a DISCLAIMER OF ALL
#  WARRANTIES.
# 
# 
CXXFLAGS := -g -Wall -W


#LIBS=-L/homes/dougs/libc -lefence
LIBS=

# Uncomment the following line for optimisation
#CXXFLAGS := -O2 -Wall -W

# Uncomment the following line to turn off the assertion checking
#CXXFLAGS := $(CXXFLAGS) -DNDEBUG

# Uncomment the following line to get useful stats from the tool
CXXFLAGS := $(CXXFLAGS) -DGETSTATS

#CXXFLAGS := $(CXXFLAGS) -DDEBUG 			// misc. debugging output
#CXXFLAGS := $(CXXFLAGS) -DTESTPOSTDOM 	// show post-dominators
#CXXFLAGS := $(CXXFLAGS) -DTESTLOOPS  
#CXXFLAGS := $(CXXFLAGS) -DTESTSOURCE
#CXXFLAGS := $(CXXFLAGS) -DTESTCFGS 
#CXXFLAGS := $(CXXFLAGS) -DTESTGRAPHS 
#CXXFLAGS := $(CXXFLAGS) -DTESTINTS 
#CXXFLAGS := $(CXXFLAGS) -DNUMBERINGS
#CXXFLAGS := $(CXXFLAGS) -DLOOPHEAD
CXXFLAGS := $(CXXFLAGS) -DCODEGEN

OBJS = StringFunctions.o TypeDefs.o Instruction.o Source.o Node.o \
		 Graphs.o DynArr.o Options.o Ast.o MemAdvise.o Stats.o

# Uncomment the following line to build the parenthesis version of the tool
BIN=ast

# Uncomment the following three lines to build the interval-theory version of the tool
# CXXFLAGS := ${CXXFLAGS} -DINTERVALS 
# OBJS := $(OBJS) Interval.o
# BIN=ast_old

${BIN}: ${OBJS}
	${CXX} ${LIBS} ${CXXFLAGS} $? -lm -o $@

%.o: %.cc 
	${CXX} ${CXXFLAGS} -c $<

depend:
		makedepend -v -I /opt/local//lib/include/g++ -I/usr/include/g++ *.h *.cc 

clean:
	${RM} *.o 

veryclean:
	${RM} *.o ${BIN} *~
# DO NOT DELETE

DynArr.o: /usr/include/string.h
# /usr/include/string.h includes:
#	iso/string_iso.h
DynArr.o: /usr/include/iso/string_iso.h
# /usr/include/iso/string_iso.h includes:
#	sys/feature_tests.h
DynArr.o: /usr/include/sys/feature_tests.h
# /usr/include/sys/feature_tests.h includes:
#	sys/isa_defs.h
DynArr.o: /usr/include/sys/isa_defs.h /opt/local/lib/include/g++/iostream.h
# /opt/local/lib/include/g++/iostream.h includes:
#	streambuf.h
DynArr.o: /opt/local/lib/include/g++/streambuf.h
# /opt/local/lib/include/g++/streambuf.h includes:
#	libio.h
DynArr.o: /opt/local/lib/include/g++/libio.h
# /opt/local/lib/include/g++/libio.h includes:
#	_G_config.h
DynArr.o: /opt/local/lib/include/g++/_G_config.h
# /opt/local/lib/include/g++/_G_config.h includes:
#	stddef.h
DynArr.o: /usr/include/stddef.h
# /usr/include/stddef.h includes:
#	sys/isa_defs.h
#	iso/stddef_iso.h
DynArr.o: /usr/include/iso/stddef_iso.h
# /usr/include/iso/stddef_iso.h includes:
#	sys/isa_defs.h
DynArr.o: /usr/include/assert.h
Graphs.o: Node.h
# Node.h includes:
#	Instruction.h
#	DynArr.h
Graphs.o: Instruction.h
# Instruction.h includes:
#	TypeDefs.h
#	DynArr.h
Graphs.o: TypeDefs.h DynArr.h
# DynArr.h includes:
#	string.h
#	iostream.h
#	assert.h
Graphs.o: /usr/include/string.h /usr/include/iso/string_iso.h
Graphs.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h
Graphs.o: /opt/local/lib/include/g++/iostream.h
Graphs.o: /opt/local/lib/include/g++/streambuf.h
Graphs.o: /opt/local/lib/include/g++/libio.h
Graphs.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Graphs.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h Source.h
# Source.h includes:
#	Instruction.h
#	DynArr.h
Instruction.o: TypeDefs.h DynArr.h /usr/include/string.h
Instruction.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
Instruction.o: /usr/include/sys/isa_defs.h
Instruction.o: /opt/local/lib/include/g++/iostream.h
Instruction.o: /opt/local/lib/include/g++/streambuf.h
Instruction.o: /opt/local/lib/include/g++/libio.h
Instruction.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Instruction.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h
Interval.o: Node.h Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
Interval.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
Interval.o: /usr/include/sys/isa_defs.h /opt/local/lib/include/g++/iostream.h
Interval.o: /opt/local/lib/include/g++/streambuf.h
Interval.o: /opt/local/lib/include/g++/libio.h
Interval.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Interval.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h
MemAdvise.o: /usr/include/malloc.h
# /usr/include/malloc.h includes:
#	sys/types.h
MemAdvise.o: /usr/include/sys/types.h
# /usr/include/sys/types.h includes:
#	sys/isa_defs.h
#	sys/feature_tests.h
#	sys/machtypes.h
#	sys/int_types.h
#	sys/select.h
MemAdvise.o: /usr/include/sys/isa_defs.h /usr/include/sys/feature_tests.h
MemAdvise.o: /usr/include/sys/machtypes.h
# /usr/include/sys/machtypes.h includes:
#	sys/feature_tests.h
MemAdvise.o: /usr/include/sys/int_types.h
# /usr/include/sys/int_types.h includes:
#	sys/isa_defs.h
MemAdvise.o: /usr/include/sys/select.h
# /usr/include/sys/select.h includes:
#	sys/feature_tests.h
#	sys/time.h
MemAdvise.o: /usr/include/sys/time.h
# /usr/include/sys/time.h includes:
#	sys/feature_tests.h
#	sys/types.h
#	time.h
#	sys/select.h
MemAdvise.o: /usr/include/time.h
# /usr/include/time.h includes:
#	sys/feature_tests.h
#	sys/types.h
#	iso/time_iso.h
MemAdvise.o: /usr/include/iso/time_iso.h
# /usr/include/iso/time_iso.h includes:
#	sys/feature_tests.h
Node.o: Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
Node.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
Node.o: /usr/include/sys/isa_defs.h /opt/local/lib/include/g++/iostream.h
Node.o: /opt/local/lib/include/g++/streambuf.h
Node.o: /opt/local/lib/include/g++/libio.h
Node.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Node.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h
Source.o: Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
Source.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
Source.o: /usr/include/sys/isa_defs.h /opt/local/lib/include/g++/iostream.h
Source.o: /opt/local/lib/include/g++/streambuf.h
Source.o: /opt/local/lib/include/g++/libio.h
Source.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Source.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h
Analysis.o: /usr/include/assert.h MemAdvise.h
# MemAdvise.h includes:
#	malloc.h
Analysis.o: /usr/include/malloc.h /usr/include/sys/types.h
Analysis.o: /usr/include/sys/isa_defs.h /usr/include/sys/feature_tests.h
Analysis.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
Analysis.o: /usr/include/sys/select.h /usr/include/sys/time.h
Analysis.o: /usr/include/time.h /usr/include/iso/time_iso.h Stats.h
Ast.o: /usr/include/stdlib.h
# /usr/include/stdlib.h includes:
#	iso/stdlib_iso.h
Ast.o: /usr/include/iso/stdlib_iso.h
# /usr/include/iso/stdlib_iso.h includes:
#	sys/feature_tests.h
Ast.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h Graphs.h
# Graphs.h includes:
#	Node.h
#	Source.h
#	Instruction.h
#	TypeDefs.h
Ast.o: Node.h Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
Ast.o: /usr/include/iso/string_iso.h /opt/local/lib/include/g++/iostream.h
Ast.o: /opt/local/lib/include/g++/streambuf.h
Ast.o: /opt/local/lib/include/g++/libio.h
Ast.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Ast.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h Source.h Options.h
Ast.o: Stats.h MemAdvise.h /usr/include/malloc.h /usr/include/sys/types.h
Ast.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
Ast.o: /usr/include/sys/select.h /usr/include/sys/time.h /usr/include/time.h
Ast.o: /usr/include/iso/time_iso.h
Dominators.o: /usr/include/assert.h /opt/local/lib/include/g++/iostream.h
Dominators.o: /opt/local/lib/include/g++/streambuf.h
Dominators.o: /opt/local/lib/include/g++/libio.h
Dominators.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Dominators.o: /usr/include/sys/isa_defs.h /usr/include/iso/stddef_iso.h
DynArr.o: /usr/include/string.h /usr/include/iso/string_iso.h
DynArr.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h
DynArr.o: /opt/local/lib/include/g++/iostream.h
DynArr.o: /opt/local/lib/include/g++/streambuf.h
DynArr.o: /opt/local/lib/include/g++/libio.h
DynArr.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
DynArr.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h DynArr.h
DynArr.o: Instruction.h TypeDefs.h
Graphs.o: Graphs.h Node.h Instruction.h TypeDefs.h DynArr.h
Graphs.o: /usr/include/string.h /usr/include/iso/string_iso.h
Graphs.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h
Graphs.o: /opt/local/lib/include/g++/iostream.h
Graphs.o: /opt/local/lib/include/g++/streambuf.h
Graphs.o: /opt/local/lib/include/g++/libio.h
Graphs.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Graphs.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h Source.h
Graphs.o: StringFunctions.h GraphsDfs.cc Dominators.cc
# Dominators.cc includes:
#	assert.h
#	iostream.h
Graphs.o: Analysis.cc
# Analysis.cc includes:
#	assert.h
#	MemAdvise.h
#	Stats.h
Graphs.o: MemAdvise.h /usr/include/malloc.h /usr/include/sys/types.h
Graphs.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
Graphs.o: /usr/include/sys/select.h /usr/include/sys/time.h
Graphs.o: /usr/include/time.h /usr/include/iso/time_iso.h Stats.h
Graphs.o: GraphsCodeGen.cc
# GraphsCodeGen.cc includes:
#	fstream.h
#	math.h
#	stdlib.h
#	Graphs.h
#	TypeDefs.h
#	StringFunctions.h
#	Options.h
#	MemAdvise.h
#	Stats.h
Graphs.o: /opt/local/lib/include/g++/fstream.h
# /opt/local/lib/include/g++/fstream.h includes:
#	iostream.h
Graphs.o: /usr/include/math.h
# /usr/include/math.h includes:
#	iso/math_iso.h
Graphs.o: /usr/include/iso/math_iso.h /usr/include/stdlib.h
Graphs.o: /usr/include/iso/stdlib_iso.h Options.h GraphsPrint.cc
# GraphsPrint.cc includes:
#	fstream.h
#	StringFunctions.h
#	Options.h
GraphsCodeGen.o: /opt/local/lib/include/g++/fstream.h
GraphsCodeGen.o: /opt/local/lib/include/g++/iostream.h
GraphsCodeGen.o: /opt/local/lib/include/g++/streambuf.h
GraphsCodeGen.o: /opt/local/lib/include/g++/libio.h
GraphsCodeGen.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
GraphsCodeGen.o: /usr/include/sys/isa_defs.h /usr/include/iso/stddef_iso.h
GraphsCodeGen.o: /usr/include/math.h /usr/include/iso/math_iso.h
GraphsCodeGen.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
GraphsCodeGen.o: /usr/include/sys/feature_tests.h Graphs.h Node.h
GraphsCodeGen.o: Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
GraphsCodeGen.o: /usr/include/iso/string_iso.h /usr/include/assert.h Source.h
GraphsCodeGen.o: StringFunctions.h Options.h MemAdvise.h
GraphsCodeGen.o: /usr/include/malloc.h /usr/include/sys/types.h
GraphsCodeGen.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
GraphsCodeGen.o: /usr/include/sys/select.h /usr/include/sys/time.h
GraphsCodeGen.o: /usr/include/time.h /usr/include/iso/time_iso.h Stats.h
GraphsDerSeq.o: /usr/include/string.h /usr/include/iso/string_iso.h
GraphsDerSeq.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h
GraphsDerSeq.o: Interval.h
# Interval.h includes:
#	Node.h
GraphsDerSeq.o: Node.h Instruction.h TypeDefs.h DynArr.h
GraphsDerSeq.o: /opt/local/lib/include/g++/iostream.h
GraphsDerSeq.o: /opt/local/lib/include/g++/streambuf.h
GraphsDerSeq.o: /opt/local/lib/include/g++/libio.h
GraphsDerSeq.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
GraphsDerSeq.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h
GraphsDerSeq.o: MemAdvise.h /usr/include/malloc.h /usr/include/sys/types.h
GraphsDerSeq.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
GraphsDerSeq.o: /usr/include/sys/select.h /usr/include/sys/time.h
GraphsDerSeq.o: /usr/include/time.h /usr/include/iso/time_iso.h Stats.h
GraphsPrint.o: /opt/local/lib/include/g++/fstream.h
GraphsPrint.o: /opt/local/lib/include/g++/iostream.h
GraphsPrint.o: /opt/local/lib/include/g++/streambuf.h
GraphsPrint.o: /opt/local/lib/include/g++/libio.h
GraphsPrint.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
GraphsPrint.o: /usr/include/sys/isa_defs.h /usr/include/iso/stddef_iso.h
GraphsPrint.o: StringFunctions.h Options.h
Instruction.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
Instruction.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h
Instruction.o: /usr/include/stdio.h
# /usr/include/stdio.h includes:
#	iso/stdio_iso.h
Instruction.o: /usr/include/iso/stdio_iso.h
# /usr/include/iso/stdio_iso.h includes:
#	sys/feature_tests.h
#	sys/va_list.h
#	stdio_tag.h
#	stdio_impl.h
Instruction.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
Instruction.o: /usr/include/stdio_impl.h
# /usr/include/stdio_impl.h includes:
#	sys/isa_defs.h
Instruction.o: /usr/include/ctype.h
# /usr/include/ctype.h includes:
#	iso/ctype_iso.h
Instruction.o: /usr/include/iso/ctype_iso.h
# /usr/include/iso/ctype_iso.h includes:
#	sys/feature_tests.h
Instruction.o: /usr/include/string.h /usr/include/iso/string_iso.h
Instruction.o: /opt/local/lib/include/g++/iostream.h
Instruction.o: /opt/local/lib/include/g++/streambuf.h
Instruction.o: /opt/local/lib/include/g++/libio.h
Instruction.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Instruction.o: /usr/include/iso/stddef_iso.h
Instruction.o: /opt/local/lib/include/g++/strstream.h
# /opt/local/lib/include/g++/strstream.h includes:
#	iostream.h
#	strfile.h
Instruction.o: /opt/local/lib/include/g++/strfile.h
# /opt/local/lib/include/g++/strfile.h includes:
#	libio.h
Instruction.o: StringFunctions.h Instruction.h TypeDefs.h DynArr.h
Instruction.o: /usr/include/assert.h
Interval.o: Node.h Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
Interval.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
Interval.o: /usr/include/sys/isa_defs.h /opt/local/lib/include/g++/iostream.h
Interval.o: /opt/local/lib/include/g++/streambuf.h
Interval.o: /opt/local/lib/include/g++/libio.h
Interval.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Interval.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h Interval.h
MemAdvise.o: MemAdvise.h /usr/include/malloc.h /usr/include/sys/types.h
MemAdvise.o: /usr/include/sys/isa_defs.h /usr/include/sys/feature_tests.h
MemAdvise.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
MemAdvise.o: /usr/include/sys/select.h /usr/include/sys/time.h
MemAdvise.o: /usr/include/time.h /usr/include/iso/time_iso.h
Node.o: Node.h Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
Node.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
Node.o: /usr/include/sys/isa_defs.h /opt/local/lib/include/g++/iostream.h
Node.o: /opt/local/lib/include/g++/streambuf.h
Node.o: /opt/local/lib/include/g++/libio.h
Node.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Node.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h NodeCodeGen.cc
# NodeCodeGen.cc includes:
#	math.h
#	stdio.h
#	Graphs.h
#	Node.h
#	TypeDefs.h
#	StringFunctions.h
#	Options.h
#	Stats.h
Node.o: /usr/include/math.h /usr/include/iso/math_iso.h /usr/include/stdio.h
Node.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
Node.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h Graphs.h Source.h
Node.o: StringFunctions.h Options.h Stats.h
NodeCodeGen.o: /usr/include/math.h /usr/include/iso/math_iso.h
NodeCodeGen.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
NodeCodeGen.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h
NodeCodeGen.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
NodeCodeGen.o: /usr/include/stdio_impl.h Graphs.h Node.h Instruction.h
NodeCodeGen.o: TypeDefs.h DynArr.h /usr/include/string.h
NodeCodeGen.o: /usr/include/iso/string_iso.h
NodeCodeGen.o: /opt/local/lib/include/g++/iostream.h
NodeCodeGen.o: /opt/local/lib/include/g++/streambuf.h
NodeCodeGen.o: /opt/local/lib/include/g++/libio.h
NodeCodeGen.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
NodeCodeGen.o: /usr/include/iso/stddef_iso.h /usr/include/assert.h Source.h
NodeCodeGen.o: StringFunctions.h Options.h Stats.h
Options.o: /opt/local/lib/include/g++/iostream.h
Options.o: /opt/local/lib/include/g++/streambuf.h
Options.o: /opt/local/lib/include/g++/libio.h
Options.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Options.o: /usr/include/sys/isa_defs.h /usr/include/iso/stddef_iso.h
Options.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
Options.o: /usr/include/sys/feature_tests.h Options.h
Source.o: /opt/local/lib/include/g++/fstream.h
Source.o: /opt/local/lib/include/g++/iostream.h
Source.o: /opt/local/lib/include/g++/streambuf.h
Source.o: /opt/local/lib/include/g++/libio.h
Source.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
Source.o: /usr/include/sys/isa_defs.h /usr/include/iso/stddef_iso.h
Source.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
Source.o: /usr/include/sys/feature_tests.h StringFunctions.h Source.h
Source.o: Instruction.h TypeDefs.h DynArr.h /usr/include/string.h
Source.o: /usr/include/iso/string_iso.h /usr/include/assert.h
Stats.o: Stats.h /usr/include/sys/time.h /usr/include/sys/feature_tests.h
Stats.o: /usr/include/sys/isa_defs.h /usr/include/sys/types.h
Stats.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
Stats.o: /usr/include/sys/select.h /usr/include/time.h
Stats.o: /usr/include/iso/time_iso.h /usr/include/sys/resource.h
# /usr/include/sys/resource.h includes:
#	sys/feature_tests.h
#	sys/types.h
#	sys/time.h
StringFunctions.o: StringFunctions.h /usr/include/string.h
StringFunctions.o: /usr/include/iso/string_iso.h
StringFunctions.o: /usr/include/sys/feature_tests.h
StringFunctions.o: /usr/include/sys/isa_defs.h /usr/include/ctype.h
StringFunctions.o: /usr/include/iso/ctype_iso.h
TypeDefs.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
TypeDefs.o: /usr/include/sys/feature_tests.h /usr/include/sys/isa_defs.h
TypeDefs.o: /usr/include/assert.h /opt/local/lib/include/g++/iostream.h
TypeDefs.o: /opt/local/lib/include/g++/streambuf.h
TypeDefs.o: /opt/local/lib/include/g++/libio.h
TypeDefs.o: /opt/local/lib/include/g++/_G_config.h /usr/include/stddef.h
TypeDefs.o: /usr/include/iso/stddef_iso.h /usr/include/string.h
TypeDefs.o: /usr/include/iso/string_iso.h /usr/include/stdlib.h
TypeDefs.o: /usr/include/iso/stdlib_iso.h TypeDefs.h
