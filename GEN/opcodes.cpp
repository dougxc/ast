/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

#include <stdlib.h>
#include <string.h>

// ### BEGIN GENERATED CODE ###

// A comparsion function for pointers to strings
int comp_fn(const char** first, const char** second) { return strcmp(*first,*second);  }

// The array of opcode strings indexed by their corresponding opcode type
const char* _opcodeStrings[] = {
	"add", "addcc", "addx", "addxcc", "and", "andcc", "andn",
	"andncc", "b", "b,a", "ba", "ba,a", "bcc", "bcc,a", "bclr",
	"bcs", "bcs,a", "be", "be,a", "bg", "bg,a", "bge", "bge,a",
	"bgeu", "bgeu,a", "bgu", "bgu,a", "bl", "bl,a", "ble", "ble,a",
	"bleu", "bleu,a", "blu", "blu,a", "bn", "bn,a", "bne", "bne,a",
	"bneg", "bneg,a", "bpos", "bpos,a", "bset", "btog", "btst", "bvc",
	"bvc,a", "bvs", "bvs,a", "call", "cb0", "cb0,a", "cb01", "cb01,a",
	"cb012", "cb012,a", "cb013", "cb013,a", "cb02", "cb02,a", "cb023",
	"cb023,a", "cb03", "cb03,a", "cb1", "cb1,a", "cb12", "cb12,a",
	"cb123", "cb123,a", "cb13", "cb13,a", "cb2", "cb2,a", "cb23",
	"cb23,a", "cb3", "cb3,a", "cba", "cba,a", "cbn", "cbn,a", "clr",
	"clrb", "clrh", "cmp", "cpop1", "cpop2", "dec", "deccc", "fabss",
	"faddd", "faddq", "fadds", "fba", "fba,a", "fbe", "fbe,a", "fbg",
	"fbg,a", "fbge", "fbge,a", "fbl", "fbl,a", "fble", "fble,a",
	"fblg", "fblg,a", "fbn", "fbn,a", "fbne", "fbne,a", "fbo",
	"fbo,a", "fbu", "fbu,a", "fbue", "fbue,a", "fbug", "fbug,a",
	"fbuge", "fbuge,a", "fbul", "fbul,a", "fbule", "fbule,a", "fcmpd",
	"fcmped", "fcmpeq", "fcmpes", "fcmpq", "fcmps", "fdivd", "fdivq",
	"fdivs", "fdmulq", "fdtoi", "fdtoq", "fdtos", "fitod", "fitoq",
	"fitos", "flush", "fmovs", "fmuld", "fmulq", "fmuls", "fnegs",
	"fpop1", "fpop2", "fqtod", "fqtoi", "fqtos", "fsmuld", "fsqrtd",
	"fsqrtq", "fsqrts", "fstod", "fstoi", "fstoq", "fsubd", "fsubq",
	"fsubs", "inc", "inccc", "jmp", "jmpl", "ld", "ldc", "ldcsr",
	"ldd", "lddc", "lddf", "ldf", "ldfsr", "ldsb", "ldsh", "ldstub",
	"ldub", "lduh", "mov", "mulscc", "neg", "nop", "not", "or",
	"orcc", "orn", "orncc", "rd", "restore", "ret", "retl", "save",
	"sdiv", "sdivcc", "set", "sethi", "sll", "smul", "smulcc",
	"sra", "srl", "st", "stb", "stbar", "stc", "stcsr", "std",
	"stdc", "stdcq", "stdf", "stf", "stfsr", "sth", "sub", "subcc",
	"subx", "subxcc", "swap", "taddcc", "taddcctv", "tst", "tsubcc",
	"tsubcctv", "udiv", "udivcc", "umul", "umulcc", "unimp", "wr",
	"xnor", "xnorcc", "xor", "xorcc", "invalid"
};

char const* Type2String(iType t) { return _opcodeStrings[t]; }

iType String2Type(const char* opString)
{
	const int NumOpcodes = 236 + 1;
	char** result = static_cast<char**>(bsearch(&opString,_opcodeStrings,NumOpcodes,sizeof(_opcodeStrings[0]),comp_fn));
	assert(result);
	return static_cast<iType>((static_cast<int>(result) - static_cast<int>(_opcodeStrings)) / sizeof(char**));

}

// ### END GENERATED CODE ###

