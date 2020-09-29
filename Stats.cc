/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

// File Stats.cpp
// Author: Doug Simon
// Purpose: implements the dtime function

#include "Stats.h"
#include <sys/time.h>
#include <sys/resource.h>

static struct rusage Rusage;

int dtime(double p[])
{
   double q;

   q = p[2];

   getrusage(RUSAGE_SELF,&Rusage);

   p[2] = (double)(Rusage.ru_utime.tv_sec);
   p[2] = p[2] + (double)(Rusage.ru_utime.tv_usec) * 1.0e-06;
   p[1] = p[2] - q;

   return 0;
}
