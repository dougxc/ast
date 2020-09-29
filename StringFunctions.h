/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

//File: string.h
//Author: Doug Simon
//Purpose: provides extra string functions

#ifndef _STRINGFUNCTIONS_
#define _STRINGFUNCTIONS_

//return the substring of str that begins at index s and is of length len
char* substr(char const* str,int s, int len);

// allocate enough space for the two given strings, concatenate them together, and
// return the result
char* concatstr(char const* s1, char const*);

// return the string made up of all the digits within the given string
char* digits(char const* str);

// our own version of the strdup function. Required to make sure the 
// memory stats gathered are correct.
char* mystrdup(char const* str);
#endif
