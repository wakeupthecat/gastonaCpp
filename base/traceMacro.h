/*
Eva library C++ (www.elxala.de)
Copyright (c) 2006-2018 Alejandro Xalabarder Aulet

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _TRACE_MACRO_HEADER_
#define _TRACE_MACRO_HEADER_

#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32

#include <sys/time.h>

#define printAppTime { \
   timeval tv; \
   gettimeofday(&tv, NULL);  \
   printf("\n%8.3f (M) ", (float)(tv.tv_sec - tvSTART.tv_sec) + 0.000001f*(tv.tv_usec - tvSTART.tv_usec)); \
}

#else

// do not print time if MS VC++ compiler
// see https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/674d34c9-b6f6-4380-bc7b-181eae99847a/timeval-struct-incorrect?forum=windowssdk
#define timeval int

#define printAppTime { printf (""); }
#define gettimeofday(p,t)

#endif // WIN32

#define DBG_ON 1
#define DBG_LEVEL_1
#define DBG_LEVEL_2_NO

#define TRACE(x) do { if (DBG_ON) { printAppTime printf x; } } while (0)
#define TRACERROR(x) do { printAppTime printf(" ERROR: "); printf x; } while (0)

#ifdef DBG_LEVEL_1
   #define TRACE1(x) do { if (DBG_ON) printAppTime printf x; } while (0)
#else
   #define TRACE1(x) do { } while (0)
#endif

#ifdef DBG_LEVEL_2
   #define TRACE2(x) do { if (DBG_ON) printAppTime printf x; } while (0)
#else
   #define TRACE2(x) do { } while (0)
#endif

static timeval tvSTART;

#endif

