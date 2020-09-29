/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

#include "StringFunctions.h"
#include <string.h>
#include <ctype.h>

char* substr(char const* str, int s, int len)
{
	int i;
	char* retStr = new char[len + 1];

	for (i = s; i < s+len; i++)
		retStr[i-s] = str[i];
	retStr[i-s] = '\0';

	return retStr;
}

char* concatstr(char const* s1, char const* s2)
{
	char* retStr = new char[strlen(s1) + strlen(s2) + 1];

	return strcat(strcpy(retStr,s1),s2);
}

char* digits(char const* str)
{
	int size = 0;
	for (int i = 0; i < (int)strlen(str); i++)
		if (isdigit(str[i]))
			size++;

	char* retStr = new char[size + 1];
	size = 0;
	for (int i = 0; i < (int)strlen(str); i++)
		if (isdigit(str[i]))
			retStr[size++] = str[i];
	retStr[size] = '\0';

	return retStr;
}

char* mystrdup (char const* str)
{
	char* retStr = new char[strlen(str) + 1];
	strcpy(retStr,str);
	return retStr;
}
