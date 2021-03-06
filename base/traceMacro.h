/*
Eva library C++ (www.elxala.de)
Copyright (c) 2006-2019 Alejandro Xalabarder Aulet

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
#include <chrono>

#define printAppTime { printf("\n%8.3f ", traceCrono.getMillisecondsFromStart () / 1000.); }

#define DBG_ON 1
#define DBG_LEVEL_1
#define DBG_LEVEL_2_NO

#define TRACE(x) do { if (DBG_ON) { printAppTime printf x; } } while (0)
#define TRACE_ERR(x) do { printAppTime printf(" ERROR: "); printf x; } while (0)

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

struct cronos_class
{
   long long getMillisecondsFrom1970 ()
   {
      auto ara = std::chrono::system_clock::now();
      auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(ara.time_since_epoch()).count();
      return milli;
   }

   static long long getMillisecondsFromStart ()
   {
      static auto start = std::chrono::system_clock::now();
      auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
      return milli;
   }
};
struct cronos_class traceCrono;


#endif

