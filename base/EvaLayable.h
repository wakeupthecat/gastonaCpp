/*
Eva library C++ (www.elxala.de)
Copyright (C) 2005-2019 Alejandro Xalabarder Aulet

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

#ifndef _EVALAYABLE_HEADER
#define _EVALAYABLE_HEADER

#include "uniRect.h"

#include <string>

using namespace std;

// data structure used by EvaLayout to store precalculated
// information about a component, that can be either a win widget or another EvaLayout
//
struct EvaLayable
{
   EvaLayable (const string & pname):
      name (pname),
      isLaidOut (false),
      cellPos (),
      iniRect ()
      {
      }

   string name;
   bool   isLaidOut;
   uniRect cellPos;  // place in the layout array mesured in array indices
                     // x  = column index
                     // dx = last column index or -1 if goes until end
                     // y   = row index
                     // dy = last row index or -1 if goes until end

   uniRect  iniRect; // initial pos and size of the component
};

#endif
